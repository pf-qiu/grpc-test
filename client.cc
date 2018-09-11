#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "kafka.grpc.pb.h"
#include <librdkafka/rdkafka.h>
#include <thread>
#include <vector>
#include <stdio.h>

using namespace KafkaConsumerServer;
using namespace grpc;

const char brokers[] = "127.0.0.1:9092";
const char topic[] = "test";
int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;
	rd_kafka_t *k = rd_kafka_new(RD_KAFKA_CONSUMER, 0, 0, 0);
	rd_kafka_brokers_add(k, brokers);
	rd_kafka_topic_t *t = rd_kafka_topic_new(k, topic, 0);
	const struct rd_kafka_metadata *meta;
	rd_kafka_resp_err_t err = rd_kafka_metadata(k, false, t, &meta, 2000);
	if (err != RD_KAFKA_RESP_ERR_NO_ERROR)
	{
		printf("Failed to get metadata: %s\n", rd_kafka_err2str(err));
		return 1;
	}

	int count = meta->topics->partition_cnt;
	printf("%d partitions\n", count);

	rd_kafka_metadata_destroy(meta);
	rd_kafka_topic_destroy(t);
	rd_kafka_destroy(k);

	auto ch = CreateChannel(argv[1], InsecureChannelCredentials());
	auto stub = Kafka::NewStub(ch);

	std::vector<std::thread> threads;
	for (int i = 0; i < count; i++)
	{
		threads.emplace_back([&]() {
			ConsumerJob job;
			job.set_brokers(brokers);
			job.set_topic(topic);
			job.set_batchsize(10);
			job.set_batchinterval(500);
			job.set_partitionid(i);
			job.set_offset(0);

			JobID id;
			{
				ClientContext ctx;
				Status s = stub->AddJob(&ctx, job, &id);
				if (!s.ok())
				{
					printf("AddJob failed\n");
					return;
				}
			}

			size_t total = 0;
			while (true)
			{
				{
					BatchData data;
					ClientContext ctx;
					auto reader = stub->ReadBatch(&ctx, id);

					while (reader->Read(&data))
					{
						auto &d = data.data();
						total += d.size();
					}

					Status s = reader->Finish();
					if (!s.ok())
					{
						printf("ReadBatch failed\n");
						return;
					}
				}

				{
					BatchInfo info;
					ClientContext ctx;
					Status s = stub->GetBatchInfo(&ctx, id, &info);
					if (!s.ok())
					{
						printf("GetBatchInfo failed\n");
						return;
					}
					if (info.eof())
						break;
				}
			}
			printf("Partition %d total %d\n", i, total);

			{
				Empty empty;
				ClientContext ctx;
				Status s = stub->DeleteJob(&ctx, id, &empty);
				if (!s.ok())
				{
					printf("DeleteJob failed\n");
					return;
				}
			}
		});
	}
	for (auto it = threads.begin(); it != threads.end(); it++)
		it->join();

	return 0;
}
