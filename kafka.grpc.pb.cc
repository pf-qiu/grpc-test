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
  "/KafkaConsumerServer.Kafka/ReadKey",
  "/KafkaConsumerServer.Kafka/ReadValue",
  "/KafkaConsumerServer.Kafka/ReadMessage",
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
  , rpcmethod_ReadKey_(Kafka_method_names[2], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_ReadValue_(Kafka_method_names[3], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_ReadMessage_(Kafka_method_names[4], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_GetBatchInfo_(Kafka_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
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

::grpc::ClientReader< ::KafkaConsumerServer::KeyMessage>* Kafka::Stub::ReadKeyRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request) {
  return ::grpc::internal::ClientReaderFactory< ::KafkaConsumerServer::KeyMessage>::Create(channel_.get(), rpcmethod_ReadKey_, context, request);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::KeyMessage>* Kafka::Stub::AsyncReadKeyRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::KeyMessage>::Create(channel_.get(), cq, rpcmethod_ReadKey_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::KeyMessage>* Kafka::Stub::PrepareAsyncReadKeyRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::KeyMessage>::Create(channel_.get(), cq, rpcmethod_ReadKey_, context, request, false, nullptr);
}

::grpc::ClientReader< ::KafkaConsumerServer::ValueMessage>* Kafka::Stub::ReadValueRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request) {
  return ::grpc::internal::ClientReaderFactory< ::KafkaConsumerServer::ValueMessage>::Create(channel_.get(), rpcmethod_ReadValue_, context, request);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::ValueMessage>* Kafka::Stub::AsyncReadValueRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::ValueMessage>::Create(channel_.get(), cq, rpcmethod_ReadValue_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::ValueMessage>* Kafka::Stub::PrepareAsyncReadValueRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::ValueMessage>::Create(channel_.get(), cq, rpcmethod_ReadValue_, context, request, false, nullptr);
}

::grpc::ClientReader< ::KafkaConsumerServer::KafkaMessage>* Kafka::Stub::ReadMessageRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request) {
  return ::grpc::internal::ClientReaderFactory< ::KafkaConsumerServer::KafkaMessage>::Create(channel_.get(), rpcmethod_ReadMessage_, context, request);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::KafkaMessage>* Kafka::Stub::AsyncReadMessageRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::KafkaMessage>::Create(channel_.get(), cq, rpcmethod_ReadMessage_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::KafkaConsumerServer::KafkaMessage>* Kafka::Stub::PrepareAsyncReadMessageRaw(::grpc::ClientContext* context, const ::KafkaConsumerServer::JobID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::KafkaConsumerServer::KafkaMessage>::Create(channel_.get(), cq, rpcmethod_ReadMessage_, context, request, false, nullptr);
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
      new ::grpc::internal::ServerStreamingHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::KeyMessage>(
          std::mem_fn(&Kafka::Service::ReadKey), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[3],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::ValueMessage>(
          std::mem_fn(&Kafka::Service::ReadValue), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[4],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Kafka::Service, ::KafkaConsumerServer::JobID, ::KafkaConsumerServer::KafkaMessage>(
          std::mem_fn(&Kafka::Service::ReadMessage), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Kafka_method_names[5],
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

::grpc::Status Kafka::Service::ReadKey(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::grpc::ServerWriter< ::KafkaConsumerServer::KeyMessage>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Kafka::Service::ReadValue(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::grpc::ServerWriter< ::KafkaConsumerServer::ValueMessage>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Kafka::Service::ReadMessage(::grpc::ServerContext* context, const ::KafkaConsumerServer::JobID* request, ::grpc::ServerWriter< ::KafkaConsumerServer::KafkaMessage>* writer) {
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

