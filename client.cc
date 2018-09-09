#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "kafka.grpc.pb.h"
#include <stdio.h>
using namespace KafkaConsumerServer;
using namespace grpc;
int main(int argc, char** argv)
{
	if (argc < 2) return 1;

	auto ch = CreateChannel(argv[1], InsecureChannelCredentials());
	auto stub = Kafka::NewStub(ch);
	
	ConsumerJob job;
	job.set_brokers("127.0.0.1:9092");
	job.set_topic("test");
	job.set_batchsize(10);
	job.set_batchinterval(500);
	job.set_partitionid(0);
	job.set_offset(0);

	JobID id;
	{
		ClientContext ctx;
		Status s = stub->AddJob(&ctx, job, &id);
		if (s.ok())
		{
			printf("%s\n", id.id().c_str());
		}
	}
	BatchData data;
	{
		ClientContext ctx;
		auto reader = stub->ReadBatch(&ctx, id);
		while (reader->Read(&data))
		{
			auto& d = data.data();
			for (auto i = d.begin(); i != d.end(); i++)
			{
				printf("%s\n", i->c_str());
			}
		}

		Status s = reader->Finish();
		if (s.ok())
		{
			printf("ok\n");
		}
		else
		{
			printf("failed\n");
		}
	}

	return 0;
}
