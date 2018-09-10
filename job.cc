#include "job.h"


using KafkaConsumerServer::ConsumerJob;

Job::Job(const ConsumerJob * job) :
	Topic(job->topic()),
	Brokers(job->brokers()),
	PartitionID(job->partitionid()),
	Offset(job->offset()),
	BatchInterval(job->batchinterval()),
	BatchSize(job->batchsize()),
	Destination(job->destination()),
	Format(job->format()),
	kafka(nullptr),
	topic(nullptr),
	messages(nullptr),
	eof(false),
	lastOffset(0),
	errorCode(0),
	data(job->batchsize())
{

}

Job::Job(Job && j) :
	Topic(std::move(j.Topic)),
	Brokers(std::move(j.Brokers)),
	PartitionID(j.PartitionID),
	Offset(j.Offset),
	BatchInterval(j.BatchInterval),
	BatchSize(j.BatchSize),
	Destination(j.Destination),
	Format(std::move(j.Format)),
	kafka(j.kafka),
	topic(j.topic),
	messages(j.messages),
	eof(j.eof),
	lastOffset(j.lastOffset),
	errorCode(j.errorCode),
	errorMessage(std::move(j.errorMessage)),
	data(std::move(j.data))
{
	j.kafka = nullptr;
	j.topic = nullptr;
	j.messages = nullptr;
}

Job::~Job()
{
	if (topic != nullptr)
	{
		rd_kafka_topic_destroy(topic);
	}

	if (kafka != nullptr)
	{
		rd_kafka_destroy(kafka);
	}

	if (messages != nullptr)
	{
		delete[] messages;
	}
}

bool Job::Init()
{
	if (kafka == nullptr)
	{
		char msg[0x100];
		rd_kafka_conf_t* conf = rd_kafka_conf_new();
		rd_kafka_conf_set(conf, "bootstrap.servers", Brokers.c_str(), 0, 0);
		rd_kafka_conf_set(conf, "session.timeout.ms", "2000", 0, 0);
		rd_kafka_conf_set(conf, "enable.auto.commit", "false", 0, 0);
		rd_kafka_conf_set(conf, "queued.min.messages", "1000000", 0, 0);

		rd_kafka_t* k = rd_kafka_new(RD_KAFKA_CONSUMER, conf, msg, sizeof(msg));
		if (k == nullptr)
		{
			return false;
		}

		rd_kafka_topic_t* t = rd_kafka_topic_new(k, Topic.c_str(), nullptr);
		if (t == nullptr)
		{
			rd_kafka_destroy(k);
			return false;
		}

		if (rd_kafka_consume_start(t, PartitionID, Offset) != RD_KAFKA_RESP_ERR_NO_ERROR)
		{
			rd_kafka_destroy(k);
			rd_kafka_topic_destroy(t);
			return false;
		}

		kafka = k;
		topic = t;

		messages = new rd_kafka_message_t*[BatchSize];
	}

	return true;
}

void Job::Start()
{
	start = std::chrono::system_clock::now();
	auto duration = std::chrono::milliseconds(BatchInterval);
	end = start + duration;
}

bool Job::Finish()
{
	return std::chrono::system_clock::now() >= end;
}

bool Job::Poll()
{
	eof = false;
	errorCode = 0;
	errorMessage.clear();
	rd_kafka_poll(kafka, 0);
	ssize_t count = rd_kafka_consume_batch(topic, PartitionID, BatchInterval, messages, BatchSize);
	if (count < 0)
	{
		return false;
	}

	data.count = count;
	for (ssize_t i = 0; i < count; i++)
	{
		rd_kafka_message_t* msg = messages[i];

		if (msg->err != 0)
		{
			if (msg->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
			{
				eof = true;
				data.count = i;
				break;
			}
			errorCode = msg->err;
			errorMessage = rd_kafka_err2str(msg->err);
			return false;
		}
		lastOffset = msg->offset;
		const char* ptr = (const char*)msg->payload;
		data[i].assign(ptr, ptr + msg->len);
		rd_kafka_message_destroy(msg);
	}

	return true;
}
