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
	started(false),
	eof(false),
	lastOffset(0),
	errorCode(0),
	flow(job->queuesize(), job->batchsize())
{

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
			errorMessage = rd_kafka_err2str(rd_kafka_last_error());
			return false;
		}

		rd_kafka_topic_t* t = rd_kafka_topic_new(k, Topic.c_str(), nullptr);
		if (t == nullptr)
		{
			errorMessage = rd_kafka_err2str(rd_kafka_last_error());
			rd_kafka_destroy(k);
			return false;
		}

		if (rd_kafka_consume_start(t, PartitionID, Offset) != RD_KAFKA_RESP_ERR_NO_ERROR)
		{
			errorMessage = rd_kafka_err2str(rd_kafka_last_error());
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

bool Job::StartBatch()
{
	if (started == true)
	{
		return false;
	}
	started = true;
	batchStart = std::chrono::system_clock::now();
	auto duration = std::chrono::milliseconds(BatchInterval);
	batchEnd = batchStart + duration;

	eof = false;
	errorCode = 0;
	errorMessage.clear();
	
	pollThread = std::thread([this]() {PollWorker(); });
	return true;
}

void Job::PollWorker()
{
	while (std::chrono::system_clock::now() < batchEnd)
	{
		rd_kafka_poll(kafka, 0);
		ssize_t count = rd_kafka_consume_batch(topic, PartitionID, BatchInterval, messages, BatchSize);
		if (count < 0)
		{
			rd_kafka_resp_err_t error = rd_kafka_last_error();
			errorCode = error;
			errorMessage = std::string("rd_kafka_consume_batch failed: ") + rd_kafka_err2str(error);
			return;
		}
		struct ParamPack {
			Job* ptr;
			size_t available;
		} param;
		param.available = count;
		param.ptr = this;
		flow.Push([](DataBatch<KeyValue>& data, void* p) {
			ParamPack* param = (ParamPack*)p;
			Job* j = param->ptr;
			data.available = param->available;
			for (size_t i = 0; i < param->available; i++)
			{
				std::unique_ptr<rd_kafka_message_t, decltype(&rd_kafka_message_destroy)> msg(j->messages[i], rd_kafka_message_destroy);
				if (msg->err != 0)
				{
					if (msg->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
					{
						j->eof = true;
						data.available = i;
						return;
					}
					j->errorCode = msg->err;
					j->errorMessage = rd_kafka_err2str(msg->err);
					return;
				}
				j->lastOffset = msg->offset;
				const char* key = (const char*)msg->key;
				const char* value = (const char*)msg->payload;

				data.batch[i].first.assign(key, key + msg->key_len);
				data.batch[i].second.assign(value, value + msg->len);
			}
		}, &param);
	}
	flow.Close();
	std::unique_lock<std::mutex> l(workerLock);
	while (activeWorker > 0)
	{
		workerCV.wait(l);
	}
	started = false;
}

bool Job::Finish()
{
	pollThread.join();
	return std::chrono::system_clock::now() >= batchEnd;
}

bool Job::Poll(ConsumeFunction f, void* p)
{
	
	return flow.Pop(f, p);
}

bool Job::AddWorker()
{
	std::lock_guard<std::mutex> l(workerLock);
	if (!started) return false;
	activeWorker++;
	return true;
}

void Job::RemoveWorker()
{
	int currentWorker = 0;
	{
		std::lock_guard<std::mutex> l(workerLock);
		activeWorker--;
		currentWorker = activeWorker;
	}

	// Notify after unlocking
	if (currentWorker == 0)
	{
		workerCV.notify_one();
	}
}