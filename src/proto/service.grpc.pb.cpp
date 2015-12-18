// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: service.proto

#include "service.pb.h"
#include "service.grpc.pb.h"

#include <grpc++/channel.h>
#include <grpc++/impl/client_unary_call.h>
#include <grpc++/impl/rpc_service_method.h>
#include <grpc++/impl/service_type.h>
#include <grpc++/support/async_unary_call.h>
#include <grpc++/support/async_stream.h>
#include <grpc++/support/sync_stream.h>
namespace souyue {
namespace recmd {
namespace service {

static const char* ContentBasedModel_method_names[] = {
  "/souyue.recmd.service.ContentBasedModel/queryCategory",
  "/souyue.recmd.service.ContentBasedModel/predictUserInterests",
  "/souyue.recmd.service.ContentBasedModel/queryNewsTrends",
  "/souyue.recmd.service.ContentBasedModel/queryUserInterests",
};

std::unique_ptr< ContentBasedModel::Stub> ContentBasedModel::NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options) {
  std::unique_ptr< ContentBasedModel::Stub> stub(new ContentBasedModel::Stub(channel));
  return stub;
}

ContentBasedModel::Stub::Stub(const std::shared_ptr< ::grpc::Channel>& channel)
  : channel_(channel), rpcmethod_queryCategory_(ContentBasedModel_method_names[0], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_predictUserInterests_(ContentBasedModel_method_names[1], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_queryNewsTrends_(ContentBasedModel_method_names[2], ::grpc::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_queryUserInterests_(ContentBasedModel_method_names[3], ::grpc::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status ContentBasedModel::Stub::queryCategory(::grpc::ClientContext* context, const ::souyue::recmd::Empty& request, ::souyue::recmd::RepeatedKeyPair* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_queryCategory_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::souyue::recmd::RepeatedKeyPair>* ContentBasedModel::Stub::AsyncqueryCategoryRaw(::grpc::ClientContext* context, const ::souyue::recmd::Empty& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::souyue::recmd::RepeatedKeyPair>(channel_.get(), cq, rpcmethod_queryCategory_, context, request);
}

::grpc::Status ContentBasedModel::Stub::predictUserInterests(::grpc::ClientContext* context, const ::souyue::recmd::Category& request, ::souyue::recmd::AlgorithmCategory* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_predictUserInterests_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::souyue::recmd::AlgorithmCategory>* ContentBasedModel::Stub::AsyncpredictUserInterestsRaw(::grpc::ClientContext* context, const ::souyue::recmd::Category& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::souyue::recmd::AlgorithmCategory>(channel_.get(), cq, rpcmethod_predictUserInterests_, context, request);
}

::grpc::Status ContentBasedModel::Stub::queryNewsTrends(::grpc::ClientContext* context, const ::souyue::recmd::Empty& request, ::souyue::recmd::CategoryDistribution* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_queryNewsTrends_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::souyue::recmd::CategoryDistribution>* ContentBasedModel::Stub::AsyncqueryNewsTrendsRaw(::grpc::ClientContext* context, const ::souyue::recmd::Empty& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::souyue::recmd::CategoryDistribution>(channel_.get(), cq, rpcmethod_queryNewsTrends_, context, request);
}

::grpc::Status ContentBasedModel::Stub::queryUserInterests(::grpc::ClientContext* context, const ::souyue::recmd::Category& request, ::souyue::recmd::CategoryDistribution* response) {
  return ::grpc::BlockingUnaryCall(channel_.get(), rpcmethod_queryUserInterests_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::souyue::recmd::CategoryDistribution>* ContentBasedModel::Stub::AsyncqueryUserInterestsRaw(::grpc::ClientContext* context, const ::souyue::recmd::Category& request, ::grpc::CompletionQueue* cq) {
  return new ::grpc::ClientAsyncResponseReader< ::souyue::recmd::CategoryDistribution>(channel_.get(), cq, rpcmethod_queryUserInterests_, context, request);
}

ContentBasedModel::AsyncService::AsyncService() : ::grpc::AsynchronousService(ContentBasedModel_method_names, 4) {}

ContentBasedModel::Service::Service() {
}

ContentBasedModel::Service::~Service() {
}

::grpc::Status ContentBasedModel::Service::queryCategory(::grpc::ServerContext* context, const ::souyue::recmd::Empty* request, ::souyue::recmd::RepeatedKeyPair* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void ContentBasedModel::AsyncService::RequestqueryCategory(::grpc::ServerContext* context, ::souyue::recmd::Empty* request, ::grpc::ServerAsyncResponseWriter< ::souyue::recmd::RepeatedKeyPair>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status ContentBasedModel::Service::predictUserInterests(::grpc::ServerContext* context, const ::souyue::recmd::Category* request, ::souyue::recmd::AlgorithmCategory* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void ContentBasedModel::AsyncService::RequestpredictUserInterests(::grpc::ServerContext* context, ::souyue::recmd::Category* request, ::grpc::ServerAsyncResponseWriter< ::souyue::recmd::AlgorithmCategory>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status ContentBasedModel::Service::queryNewsTrends(::grpc::ServerContext* context, const ::souyue::recmd::Empty* request, ::souyue::recmd::CategoryDistribution* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void ContentBasedModel::AsyncService::RequestqueryNewsTrends(::grpc::ServerContext* context, ::souyue::recmd::Empty* request, ::grpc::ServerAsyncResponseWriter< ::souyue::recmd::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(2, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::Status ContentBasedModel::Service::queryUserInterests(::grpc::ServerContext* context, const ::souyue::recmd::Category* request, ::souyue::recmd::CategoryDistribution* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

void ContentBasedModel::AsyncService::RequestqueryUserInterests(::grpc::ServerContext* context, ::souyue::recmd::Category* request, ::grpc::ServerAsyncResponseWriter< ::souyue::recmd::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
  AsynchronousService::RequestAsyncUnary(3, context, request, response, new_call_cq, notification_cq, tag);
}

::grpc::RpcService* ContentBasedModel::Service::service() {
  if (service_) {
    return service_.get();
  }
  service_ = std::unique_ptr< ::grpc::RpcService>(new ::grpc::RpcService());
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      ContentBasedModel_method_names[0],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< ContentBasedModel::Service, ::souyue::recmd::Empty, ::souyue::recmd::RepeatedKeyPair>(
          std::mem_fn(&ContentBasedModel::Service::queryCategory), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      ContentBasedModel_method_names[1],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< ContentBasedModel::Service, ::souyue::recmd::Category, ::souyue::recmd::AlgorithmCategory>(
          std::mem_fn(&ContentBasedModel::Service::predictUserInterests), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      ContentBasedModel_method_names[2],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< ContentBasedModel::Service, ::souyue::recmd::Empty, ::souyue::recmd::CategoryDistribution>(
          std::mem_fn(&ContentBasedModel::Service::queryNewsTrends), this)));
  service_->AddMethod(new ::grpc::RpcServiceMethod(
      ContentBasedModel_method_names[3],
      ::grpc::RpcMethod::NORMAL_RPC,
      new ::grpc::RpcMethodHandler< ContentBasedModel::Service, ::souyue::recmd::Category, ::souyue::recmd::CategoryDistribution>(
          std::mem_fn(&ContentBasedModel::Service::queryUserInterests), this)));
  return service_.get();
}


}  // namespace souyue
}  // namespace recmd
}  // namespace service

