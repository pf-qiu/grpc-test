#pragma once

#include "utils.h"
#include "kafka.pb.h"
#include <librdkafka/rdkafka.h>
#include <mutex>
#include <string>
#include <chrono>
#include <thread>
#include <condition_variable>

template <typename T>
struct DataBatch
{
	DataBatch(size_t batchSize) : batch(new T[batchSize]) {}
	DataBatch(const DataBatch&) = delete;
	DataBatch(DataBatch&& other) = delete;

	~DataBatch()
	{
		if (batch != nullptr)
			delete[] batch;
	}

	T* batch;
	size_t available;
};


template <typename T, typename alloc = std::allocator<T>>
class DataFlow
{
public:
	typedef void(*PushFunction)(T&, void*);
	typedef void(*PopFunction)(T&, void*);

	DataFlow(size_t queueSize, size_t batchSize) :
		queueSize(queueSize),
		batchSize(batchSize),
		closed(false),
		next(0),
		current(0)
	{
		auto m = alloc();
		q = std::allocator_traits<alloc>::allocate(m, queueSize);
		for (size_t i = 0; i < queueSize; i++)
		{
			std::allocator_traits<alloc>::construct(m, q + i, batchSize);
		}
	}
	~DataFlow()
	{
		auto m = alloc();
		for (size_t i = 0; i < queueSize; i++)
		{
			std::allocator_traits<alloc>::destroy(m, q + i);
		}
		std::allocator_traits<alloc>::deallocate(m, q, queueSize);
	}
	DataFlow(const DataFlow&) = delete;
	DataFlow(DataFlow&&) = delete;

	void Push(PushFunction f, void* p)
	{
		{
			std::unique_lock<std::mutex> l(m);
			while (next == current + queueSize)
			{
				producer.wait(l);
				if (closed) return;
			}
			f(q[next % queueSize], p);
			next++;
		}
		consumer.notify_one();
	}

	bool Pop(PopFunction f, void* p)
	{
		{
			std::unique_lock<std::mutex> l(m);
			while (next == current)
			{
				if (closed)
				{
					return false;
				}
				consumer.wait(l);
			}
			f(q[current % queueSize], p);
			current++;
		}
		producer.notify_one();
		return true;
	}
	void Close()
	{
		closed = true;
		std::unique_lock<std::mutex> l(m);
		while (next != current)
		{
			producer.wait(l);
		}
	}

private:
	bool closed;
	const size_t queueSize, batchSize;
	size_t current, next;

	T* q;
	std::mutex m;
	std::condition_variable producer, consumer;
};

class Job
{
public:
	typedef std::pair<std::string, std::string> KeyValue;
	typedef DataFlow<DataBatch<KeyValue>>::PopFunction ConsumeFunction;
	typedef DataBatch<KeyValue> ElementType;

	Job(const KafkaConsumerServer::ConsumerJob* job);
	Job(const Job&) = delete;
	Job(Job&&) = delete;
	~Job();

	bool Init();
	bool StartBatch();
	
	bool Finish();
	bool Poll(ConsumeFunction, void*);
	bool AddWorker();
	void RemoveWorker();

	//bool Started() const { return started; }
	bool PartitionEOF() const { return eof; }
	int64_t LastOffset() const { return lastOffset; }
	int ErrorCode() const { return errorCode; }
	const std::string& ErrorMessage() const { return errorMessage; }

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

	bool started;
	std::chrono::system_clock::time_point batchStart, batchEnd;

	std::thread pollThread;
	void PollWorker();
	DataFlow<DataBatch<KeyValue>> flow;

	bool eof;
	int64_t lastOffset;
	int errorCode;
	std::string errorMessage;

	int activeWorker;
	std::mutex workerLock;
	std::condition_variable workerCV;
};
