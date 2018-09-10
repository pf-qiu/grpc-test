// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: kafka.proto

#include "kafka.pb.h"
#include "kafka.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace KafkaConsumerServer {

static const char* Kafka_method_names[] = {
  "/KafkaConsumerServer.Kafka/AddJob",
  "/KafkaConsumerServer.Kafka/DeleteJob",
  "/KafkaConsumerServer.Kafka/ReadBatch",
  "/KafkaConsumerServer.Kafka/GetBatchInfo",
};

std::unique_ptr< Kafka::Stub> Kafka::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Kafka::Stub> stub(new Kafka::Stub(channel));
  return stub;
}

Kafka::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_AddJob_(Kafka_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_DeleteJob_(Kafka_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ReadBatch_(Kafka_method_names[2], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_GetBatchInfo_(Kafka_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Kafka::Stub::AddJob(::grpc::ClientContext* context, const ::KafkaConsumerServer::ConsumerJob& request, ::KafkaConsumerServer::JobID* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_AddJob_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::JobID>* Kafka::Stub::AsyncAddJobRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::ConsumerJob& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::JobID>::Create(channel_.get(), cq, rpcmethod_AddJob_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::JobID>* Kafka::Stub::PrepareAsyncAddJobRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::ConsumerJob& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::JobID>::Create(channel_.get(), cq, rpcmethod_AddJob_, context, request, false);
}

::grpc::Status Kafka::Stub::DeleteJob(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::KafkaConsumerServer::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_DeleteJob_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::Empty>* Kafka::Stub::AsyncDeleteJobRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::Empty>::Create(channel_.get(), cq, rpcmethod_DeleteJob_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::Empty>* Kafka::Stub::PrepareAsyncDeleteJobRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::Empty>::Create(channel_.get(), cq, rpcmethod_DeleteJob_, context, request, false);
}

::grpc::ClientReader< ::KafkaConsumerServer::BatchData>* Kafka::Stub::ReadBatchRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request) {
  return ::grpc::internal::ClientReaderFactory< ::KafkaConsumerServer::BatchData>::Create(channel_.get(), rpcmethod_ReadBatch_, context, request);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::BatchData>* Kafka::Stub::AsyncReadBatchRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::BatchData>::Create(channel_.get(), cq, rpcmethod_ReadBatch_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::BatchData>* Kafka::Stub::PrepareAsyncReadBatchRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::BatchData>::Create(channel_.get(), cq, rpcmethod_ReadBatch_, context, request, false, nullptr);
}

::grpc::Status Kafka::Stub::GetBatchInfo(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::KafkaConsumerServer::BatchInfo* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetBatchInfo_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::BatchInfo>* Kafka::Stub::AsyncGetBatchInfoRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::BatchInfo>::Create(channel_.get(), cq, rpcmethod_GetBatchInfo_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::KafkaConsumerServer::BatchInfo>* Kafka::Stub::PrepareAsyncGetBatchInfoRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::KafkaConsumerServer::BatchInfo>::Create(channel_.get(), cq, rpcmethod_GetBatchInfo_, context, request, false);
}

Kafka::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Kafka::Service, ::KafkaConsumerServer::ConsumerJob, ::KafkaConsumerServer::JobID>(
          std::mem_fn(&Kafka::Service::AddJob), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::Empty>(
          std::mem_fn(&Kafka::Service::DeleteJob), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[2],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::BatchData>(
          std::mem_fn(&Kafka::Service::ReadBatch), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::BatchInfo>(
          std::mem_fn(&Kafka::Service::GetBatchInfo), this)));
}

Kafka::Service::~Service() {
}

::grpc::Status Kafka::Service::AddJob(::grpc::ServerContext* context, const ::KafkaConsumerServer::ConsumerJob* request, ::KafkaConsumerServer::JobID* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Kafka::Service::DeleteJob(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::KafkaConsumerServer::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Kafka::Service::ReadBatch(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::grpc::ServerWriter< ::KafkaConsumerServer::BatchData>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Kafka::Service::GetBatchInfo(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::KafkaConsumerServer::BatchInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace KafkaConsumerServer

