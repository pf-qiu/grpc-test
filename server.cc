#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include <mutex>
#include <map>

#include "kafka.grpc.pb.h"
#include "utils.h"
#include "job.h"

using std::mutex;
using std::lock_guard;
using std::unique_lock;
using std::map;
using std::unique_ptr;

using namespace grpc;
using namespace KafkaConsumerServer;
using namespace GeneralUtils;

class ConsumerServer : public Kafka::Service
{
public:
	ConsumerServer() {}
	virtual ~ConsumerServer() {}
	virtual Status AddJob(ServerContext* context, const ConsumerJob* request, JobID* response)
	{
		if (!CheckAddJob(request))
		{
			return Status(StatusCode::INVALID_ARGUMENT, "");
		}
		lock_guard<mutex> l(m);
		string id = g.Next();
		Job j(request);
		if (j.Init())
		{
			jobs.emplace(id, std::move(j));
			locks.emplace(id, std::make_unique<mutex>());
			response->set_id(std::move(id));
			return Status::OK;
		}
		else
		{
			return Status(StatusCode::INTERNAL, j.GetLastKafkaError());
		}
	}
	virtual Status DeleteJob(ServerContext* context, const JobID* request, Empty* response)
	{
		lock_guard<mutex> g(m);
		const string& id = request->id();
		auto it = jobs.find(id);
		if (it == jobs.end())
		{
			return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
		}

		auto l = locks.find(id);
		{
			lock_guard<mutex> g(*l->second);
			jobs.erase(it);
		}
		locks.erase(l);
		return Status::OK;
	}
	virtual Status ReadBatch(ServerContext* context, const JobID* request, ServerWriter< BatchData>* writer)
	{
		const string& id = request->id();
		auto it = jobs.end();
		auto l = locks.end();
		{
			lock_guard<mutex> g(m);
			it = jobs.find(id);
			if (it == jobs.end())
			{
				m.unlock();
				return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
			}
			/* Acquire per job lock before releasing global lock.
			 * Ensure only one call and prevent job from being deleted.
			 */
			l = locks.find(id);
			l->second->lock();
		}
		unique_lock<mutex> ul(*l->second, std::adopt_lock);

		Job& job = it->second;
		job.Start();
		while (!job.Finish())
		{
			if (!job.Poll())
			{
				return Status(StatusCode::INTERNAL, job.GetLastKafkaError());
			}

			BatchData batch;
			auto& data = job.GetData();
			for (size_t i = 0; i < data.count; i++)
			{
				batch.add_data(std::move(data[i]));
			}
			writer->Write(batch);
		}

		return Status::OK;
	}
	virtual Status GetBatchInfo(ServerContext* context, const JobID* request, BatchInfo* response)
	{
		const string& id = request->id();
		auto it = jobs.end();
		auto l = locks.end();
		{
			lock_guard<mutex> g(m);
			it = jobs.find(id);
			if (it == jobs.end())
			{
				m.unlock();
				return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
			}
			/* Acquire per job lock before releasing global lock.
			 * Ensure only one call and prevent job from being deleted.
			 */
			l = locks.find(id);
			l->second->lock();
		}
		unique_lock<mutex> ul(*l->second, std::adopt_lock);

		Job& job = it->second;
		response->set_eof(job.eof);
		response->set_lastoffset(job.lastOffset);
		if (job.errorCode != 0)
		{
			response->set_errorcode(job.errorCode);
			response->set_errormessage(job.errorMessage);
		}
		return Status::OK;
	}
private:
	bool CheckAddJob(const ConsumerJob* request)
	{
		if (request->brokers().empty()) return false;
		if (request->topic().empty()) return false;
		if (request->partitionid() < 0) return false;
		if (request->offset() < 0) return false;
		if (request->batchsize() <= 0) return false;
		if (request->batchinterval() < 200) return false;
		return true;
	}

	IDGenerator g;
	map<string, Job> jobs;
	map<string, unique_ptr<mutex>> locks;
	mutex m;
};

int main(int argc, char** argv)
{
	if (argc < 2) return 1;

	ConsumerServer server;
	ServerBuilder builder;
	int port = 0;
	builder.AddListeningPort(argv[1], InsecureServerCredentials(), &port);
	builder.RegisterService(&server);
	auto s = builder.BuildAndStart();
	if (!s) return 1;

	s->Wait();
	return 0;
}
