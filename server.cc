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

template<typename T>
void FillMessage(T&, Job::KeyValue&) {}

template<>
void FillMessage<KeyMessage>(KeyMessage& msg, Job::KeyValue& kv)
{
	msg.add_data(std::move(kv.first));
}

template<>
void FillMessage<ValueMessage>(ValueMessage& msg, Job::KeyValue& kv)
{
	msg.add_data(std::move(kv.second));
}

template<>
void FillMessage<KafkaMessage>(KafkaMessage& msg, Job::KeyValue& kv)
{
	auto m = msg.add_data();
	m->set_key(std::move(kv.first));
	m->set_value(std::move(kv.second));
}

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
		auto j = std::make_unique<Job>(request);
		if (j->Init())
		{
			jobs.emplace(id, std::move(j));
			response->set_id(std::move(id));
			return Status::OK;
		}
		else
		{
			return Status(StatusCode::INTERNAL, j->ErrorMessage());
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
		it->second->Finish();
		jobs.erase(it);
		return Status::OK;
	}

	virtual Status StartBatch(ServerContext* context, const JobID* request, Empty* response)
	{
		const string& id = request->id();
		lock_guard<mutex> g(m);
		auto it = jobs.find(id);
		if (it == jobs.end())
		{
			return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
		}

		if (!it->second->StartBatch())
		{
			return Status(StatusCode::INVALID_ARGUMENT, "Job is already started");
		}

		return Status::OK;
	}

	virtual Status ReadKey(ServerContext* context, const JobID* request, ServerWriter<KeyMessage>* writer)
	{
		return ReadPart<KeyMessage>(request, writer);
	}
	virtual Status ReadValue(ServerContext* context, const JobID* request, ServerWriter<ValueMessage>* writer)
	{
		return ReadPart<ValueMessage>(request, writer);
	}
	virtual Status ReadMessage(ServerContext* context, const JobID* request, ServerWriter<KafkaMessage>* writer)
	{
		return ReadPart<KafkaMessage>(request, writer);
	}

	virtual Status GetBatchInfo(ServerContext* context, const JobID* request, BatchInfo* response)
	{
		const string& id = request->id();
		auto it = jobs.end();
		{
			lock_guard<mutex> g(m);
			it = jobs.find(id);
			if (it == jobs.end())
			{
				return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
			}
		}
		Job& job = *it->second;
		response->set_eof(job.PartitionEOF());
		response->set_lastoffset(job.LastOffset());
		if (job.ErrorCode() != 0)
		{
			response->set_errorcode(job.ErrorCode());
			response->set_errormessage(job.ErrorMessage());
		}
		return Status::OK;
	}
private:
	template<typename T>
	Status ReadPart(const JobID* request, ServerWriter<T>* writer)
	{
		const string& id = request->id();
		auto it = jobs.end();
		{
			lock_guard<mutex> g(m);
			it = jobs.find(id);
			if (it == jobs.end())
			{
				return Status(StatusCode::INVALID_ARGUMENT, "Invalid JobID");
			}

			if (!it->second->AddWorker())
			{
				return Status(StatusCode::INVALID_ARGUMENT, "Job isn't started");
			}
		}

		std::vector<Job::KeyValue> data;
		Job& job = *it->second;
		job.Poll([](Job::ElementType& e, void* p) {
			std::vector<Job::KeyValue>* param = (std::vector<Job::KeyValue>*)p;
			param->reserve(e.available);
			for (size_t i = 0; i < e.available; i++)
			{
				param->emplace_back(std::move(e.batch[i]));
			}
		}, &data);
		for (auto& kv : data)
		{
			T msg;
			FillMessage(msg, kv);
			if (!writer->Write(msg))
			{
				break;
			}
		}

		it->second->RemoveWorker();
		return Status::OK;
	}
	bool CheckAddJob(const ConsumerJob* request)
	{
		if (request->brokers().empty()) return false;
		if (request->topic().empty()) return false;
		if (request->partitionid() < 0) return false;
		if (request->offset() < 0) return false;
		if (request->batchsize() <= 0) return false;
		if (request->batchinterval() < 200) return false;
		if (request->queuesize() <= 0) return false;
		return true;
	}

	IDGenerator g;
	map<string, unique_ptr<Job>> jobs;
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
