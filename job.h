#pragma once

#include "utils.h"
#include "kafka.pb.h"
#include <librdkafka/rdkafka.h>
#include <mutex>
#include <string>
#include <chrono>


class Job
{
public:
	typedef std::pair<std::string, std::string> KeyValue;

	Job(const KafkaConsumerServer::ConsumerJob* job);
	Job(Job&& j);
	~Job();

	bool Init();
	void Start();
	bool Finish();
	bool Poll();

	const char* GetLastKafkaError()
	{
		return rd_kafka_err2str(rd_kafka_last_error());
	}
	const GeneralUtils::Array<KeyValue>& GetData() const { return data; }

	bool eof;
	int64_t lastOffset;
	int errorCode;
	std::string errorMessage;

	
private:
	std::string Topic;
	std::string Brokers;
	int PartitionID;
	int64_t Offset;
	int BatchInterval;
	int BatchSize;
	std::string Destination;
	std::string Format;

	rd_kafka_t* kafka;
	rd_kafka_topic_t* topic;
	rd_kafka_message_t** messages;

	std::chrono::system_clock::time_point start, end;

	
	GeneralUtils::Array<KeyValue> data;
};