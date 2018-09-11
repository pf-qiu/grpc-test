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
const int channels = 4;
const int basePort = 6000;
size_t totalMessages, totalBytes;
std::unique_ptr<Kafka::Stub> stubs[channels];
std::mutex m;
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
			job.set_batchsize(10);
			job.set_batchinterval(500);
			job.set_partitionid(part);
			job.set_offset(0);

			auto& stub = stubs[part % channels];
			JobID id;
			{
				ClientContext ctx;
				Status s = stub->AddJob(&ctx, job, &id);
				q++;
				if (!s.ok())
				{
					cout << "AddJob failed" << endl;
					return;
				}
			}

			size_t messages = 0;
			size_t bytes = 0;
			while (true)
			{
				{
					BatchData data;
					ClientContext ctx;
					auto reader = stub->ReadBatch(&ctx, id);
					q++;
					while (reader->Read(&data))
					{
						q++;
						auto &d = data.data();
						messages += d.size();
						for (auto it = d.begin(); it != d.end(); it++)
						{
							bytes += it->size();
						}
					}

					Status s = reader->Finish();
					q++;
					if (!s.ok())
					{
						cout << "ReadBatch failed" << endl;
						return;
					}
				}

				{
					BatchInfo info;
					ClientContext ctx;
					Status s = stub->GetBatchInfo(&ctx, id, &info);
					q++;
					if (!s.ok())
					{
						cout << "GetBatchInfo failed" << endl;
						return;
					}
					if (info.eof())
						break;
				}
			}

			{
				Empty empty;
				ClientContext ctx;
				q++;
				Status s = stub->DeleteJob(&ctx, id, &empty);
				if (!s.ok())
				{
					cout << "DeleteJob failed" << endl;
					return;
				}
			}
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
