// Generated by the gRPC protobuf plugin.
// If you make any local change, they will be lost.
// source: service.proto
#ifndef GRPC_service_2eproto__INCLUDED
#define GRPC_service_2eproto__INCLUDED

#include "service.pb.h"

#include <grpc++/support/async_stream.h>
#include <grpc++/impl/rpc_method.h>
#include <grpc++/impl/proto_utils.h>
#include <grpc++/impl/service_type.h>
#include <grpc++/support/async_unary_call.h>
#include <grpc++/support/status.h>
#include <grpc++/support/stub_options.h>
#include <grpc++/support/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class RpcService;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace proto {

class ContentBasedModel GRPC_FINAL {
 public:
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status queryCategory(::grpc::ClientContext* context, const ::Empty& request, ::RepeatedKeyPair* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RepeatedKeyPair>> AsyncqueryCategory(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::RepeatedKeyPair>>(AsyncqueryCategoryRaw(context, request, cq));
    }
    virtual ::grpc::Status marshalUserInterests(::grpc::ClientContext* context, const ::Category& request, ::AlgorithmCategory* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AlgorithmCategory>> AsyncmarshalUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AlgorithmCategory>>(AsyncmarshalUserInterestsRaw(context, request, cq));
    }
    virtual ::grpc::Status predictUserInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>> AsyncpredictUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>>(AsyncpredictUserInterestsRaw(context, request, cq));
    }
    virtual ::grpc::Status queryNewsTrends(::grpc::ClientContext* context, const ::Empty& request, ::CategoryDistribution* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>> AsyncqueryNewsTrends(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>>(AsyncqueryNewsTrendsRaw(context, request, cq));
    }
    virtual ::grpc::Status queryCurrentTrends(::grpc::ClientContext* context, const ::Empty& request, ::CategoryDistribution* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>> AsyncqueryCurrentTrends(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>>(AsyncqueryCurrentTrendsRaw(context, request, cq));
    }
    virtual ::grpc::Status queryUserInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>> AsyncqueryUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>>(AsyncqueryUserInterestsRaw(context, request, cq));
    }
    virtual ::grpc::Status queryUserCurrentInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>> AsyncqueryUserCurrentInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>>(AsyncqueryUserCurrentInterestsRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::RepeatedKeyPair>* AsyncqueryCategoryRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::AlgorithmCategory>* AsyncmarshalUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>* AsyncpredictUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>* AsyncqueryNewsTrendsRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>* AsyncqueryCurrentTrendsRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>* AsyncqueryUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::CategoryDistribution>* AsyncqueryUserCurrentInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub GRPC_FINAL : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::Channel>& channel);
    ::grpc::Status queryCategory(::grpc::ClientContext* context, const ::Empty& request, ::RepeatedKeyPair* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RepeatedKeyPair>> AsyncqueryCategory(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::RepeatedKeyPair>>(AsyncqueryCategoryRaw(context, request, cq));
    }
    ::grpc::Status marshalUserInterests(::grpc::ClientContext* context, const ::Category& request, ::AlgorithmCategory* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AlgorithmCategory>> AsyncmarshalUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AlgorithmCategory>>(AsyncmarshalUserInterestsRaw(context, request, cq));
    }
    ::grpc::Status predictUserInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>> AsyncpredictUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>>(AsyncpredictUserInterestsRaw(context, request, cq));
    }
    ::grpc::Status queryNewsTrends(::grpc::ClientContext* context, const ::Empty& request, ::CategoryDistribution* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>> AsyncqueryNewsTrends(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>>(AsyncqueryNewsTrendsRaw(context, request, cq));
    }
    ::grpc::Status queryCurrentTrends(::grpc::ClientContext* context, const ::Empty& request, ::CategoryDistribution* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>> AsyncqueryCurrentTrends(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>>(AsyncqueryCurrentTrendsRaw(context, request, cq));
    }
    ::grpc::Status queryUserInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>> AsyncqueryUserInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>>(AsyncqueryUserInterestsRaw(context, request, cq));
    }
    ::grpc::Status queryUserCurrentInterests(::grpc::ClientContext* context, const ::Category& request, ::CategoryDistribution* response) GRPC_OVERRIDE;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>> AsyncqueryUserCurrentInterests(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>>(AsyncqueryUserCurrentInterestsRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::Channel> channel_;
    ::grpc::ClientAsyncResponseReader< ::RepeatedKeyPair>* AsyncqueryCategoryRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::AlgorithmCategory>* AsyncmarshalUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>* AsyncpredictUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>* AsyncqueryNewsTrendsRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>* AsyncqueryCurrentTrendsRaw(::grpc::ClientContext* context, const ::Empty& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>* AsyncqueryUserInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    ::grpc::ClientAsyncResponseReader< ::CategoryDistribution>* AsyncqueryUserCurrentInterestsRaw(::grpc::ClientContext* context, const ::Category& request, ::grpc::CompletionQueue* cq) GRPC_OVERRIDE;
    const ::grpc::RpcMethod rpcmethod_queryCategory_;
    const ::grpc::RpcMethod rpcmethod_marshalUserInterests_;
    const ::grpc::RpcMethod rpcmethod_predictUserInterests_;
    const ::grpc::RpcMethod rpcmethod_queryNewsTrends_;
    const ::grpc::RpcMethod rpcmethod_queryCurrentTrends_;
    const ::grpc::RpcMethod rpcmethod_queryUserInterests_;
    const ::grpc::RpcMethod rpcmethod_queryUserCurrentInterests_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::Channel>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::SynchronousService {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status queryCategory(::grpc::ServerContext* context, const ::Empty* request, ::RepeatedKeyPair* response);
    virtual ::grpc::Status marshalUserInterests(::grpc::ServerContext* context, const ::Category* request, ::AlgorithmCategory* response);
    virtual ::grpc::Status predictUserInterests(::grpc::ServerContext* context, const ::Category* request, ::CategoryDistribution* response);
    virtual ::grpc::Status queryNewsTrends(::grpc::ServerContext* context, const ::Empty* request, ::CategoryDistribution* response);
    virtual ::grpc::Status queryCurrentTrends(::grpc::ServerContext* context, const ::Empty* request, ::CategoryDistribution* response);
    virtual ::grpc::Status queryUserInterests(::grpc::ServerContext* context, const ::Category* request, ::CategoryDistribution* response);
    virtual ::grpc::Status queryUserCurrentInterests(::grpc::ServerContext* context, const ::Category* request, ::CategoryDistribution* response);
    ::grpc::RpcService* service() GRPC_OVERRIDE GRPC_FINAL;
   private:
    std::unique_ptr< ::grpc::RpcService> service_;
  };
  class AsyncService GRPC_FINAL : public ::grpc::AsynchronousService {
   public:
    explicit AsyncService();
    ~AsyncService() {};
    void RequestqueryCategory(::grpc::ServerContext* context, ::Empty* request, ::grpc::ServerAsyncResponseWriter< ::RepeatedKeyPair>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestmarshalUserInterests(::grpc::ServerContext* context, ::Category* request, ::grpc::ServerAsyncResponseWriter< ::AlgorithmCategory>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestpredictUserInterests(::grpc::ServerContext* context, ::Category* request, ::grpc::ServerAsyncResponseWriter< ::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestqueryNewsTrends(::grpc::ServerContext* context, ::Empty* request, ::grpc::ServerAsyncResponseWriter< ::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestqueryCurrentTrends(::grpc::ServerContext* context, ::Empty* request, ::grpc::ServerAsyncResponseWriter< ::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestqueryUserInterests(::grpc::ServerContext* context, ::Category* request, ::grpc::ServerAsyncResponseWriter< ::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
    void RequestqueryUserCurrentInterests(::grpc::ServerContext* context, ::Category* request, ::grpc::ServerAsyncResponseWriter< ::CategoryDistribution>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag);
  };
};

}  // namespace proto


#endif  // GRPC_service_2eproto__INCLUDED
