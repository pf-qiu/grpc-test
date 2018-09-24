#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "kafka.grpc.pb.h"
#include <librdkafka/rdkafka.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include <sstream>

using namespace KafkaConsumerServer;
using namespace grpc;
using std::cout;
using std::endl;

const char* brokers;
const char* topic;
const int channels = 1;
const int basePort = 6000;
size_t totalMessages, totalBytes;
std::unique_ptr<Kafka::Stub> stubs[channels];
std::mutex m;

JobID AddJob(const ConsumerJob& job, Kafka::Stub* stub)
{
	ClientContext ctx;
	JobID id;
	stub->AddJob(&ctx, job, &id);
	return id;
}
bool DeleteJob(const JobID& id, Kafka::Stub* stub)
{
	Empty empty;
	ClientContext ctx;
	Status s = stub->DeleteJob(&ctx, id, &empty);
	if (!s.ok())
	{
		cout << "DeleteJob failed" << endl;
		return false;
	}
	return true;
}


void StartBatch(const JobID& id, Kafka::Stub* stub)
{
	ClientContext ctx;
	Empty empty;
	stub->StartBatch(&ctx, id, &empty);
}

bool ReadValue(const JobID& id, Kafka::Stub* stub, size_t& messages, size_t& bytes)
{
	ValueMessage data;
	ClientContext ctx;
	auto reader = stub->ReadValue(&ctx, id);
	while (reader->Read(&data))
	{
		auto &d = data.data();
		messages += d.size();
		for (auto it = d.begin(); it != d.end(); it++)
			bytes += it->size();
	}

	Status s = reader->Finish();
	if (!s.ok())
	{
		cout << "ReadBatch failed" << endl;
		return false;
	}
	return true;
}

bool GetBatchInfo(const JobID& id, Kafka::Stub* stub)
{
	BatchInfo info;
	ClientContext ctx;
	Status s = stub->GetBatchInfo(&ctx, id, &info);
	if (!s.ok())
	{
		cout << "GetBatchInfo failed" << endl;
		return false;
	}
	if (info.eof())
		return false;
	return true;
}

int main(int argc, char **argv)
{
	if (argc < 3) return 1;
	brokers = argv[1];
	topic = argv[2];
	rd_kafka_t *k = rd_kafka_new(RD_KAFKA_CONSUMER, 0, 0, 0);
	rd_kafka_brokers_add(k, brokers);
	rd_kafka_topic_t *t = rd_kafka_topic_new(k, topic, 0);
	const struct rd_kafka_metadata *meta;
	rd_kafka_resp_err_t err = rd_kafka_metadata(k, false, t, &meta, 2000);
	if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
	{
		cout << "Failed to get metadata: " << rd_kafka_err2str(err) << endl;
		return 1;
	}

	int count = meta->topics->partition_cnt;
	cout << count << " partitions" << endl;

	rd_kafka_metadata_destroy(meta);
	rd_kafka_topic_destroy(t);
	rd_kafka_destroy(k);

	for (int i = 0; i < channels; i++)
	{
		std::stringstream address;
		address << "127.0.0.1:";
		address << basePort + i;
		auto ch = CreateChannel(address.str(), InsecureChannelCredentials());
		stubs[i] = Kafka::NewStub(ch);
	}

	std::vector<std::thread> threads;

	for (int i = 0; i < count; i++)
	{
		threads.emplace_back([](int part) {
			int q = 0;
			ConsumerJob job;
			job.set_brokers(brokers);
			job.set_topic(topic);
			job.set_batchsize(1024);
			job.set_batchinterval(500);
			job.set_partitionid(part);
			job.set_offset(0);
			job.set_queuesize(2);

			Kafka::Stub* stub = stubs[part % channels].get();
			JobID id = AddJob(job, stub);

			size_t messages = 0;
			size_t bytes = 0;
			while (true)
			{
				StartBatch(id, stub);
				ReadValue(id, stub, messages, bytes);
				if (!GetBatchInfo(id, stub)) break;
			}

			DeleteJob(id, stub);
			std::lock_guard<std::mutex> l(m);
			cout << "Partition: " << part << ", query: " << q << ", messages: " << messages << ", bytes: " << bytes << endl;
			totalMessages += messages;
			totalBytes += bytes;
		}, i);
	}
	for (auto it = threads.begin(); it != threads.end(); it++)
		it->join();

	cout << "Total messages: " << totalMessages << endl;
	cout << "Total bytes: " << totalBytes << endl;
	return 0;
}
