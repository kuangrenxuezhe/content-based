#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GRPC_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GRPC_H

#include "core/content_based.h"
#include "proto/service.grpc.pb.h"

namespace souyue {
  namespace recmd {
    // 实现GRPC接口
    class ServiceGrpc: public proto::ContentBasedModel::Service {
      public:
        ServiceGrpc(ContentBased* content_based);
        virtual ~ServiceGrpc();

      public:
        virtual grpc::Status queryCategory(grpc::ServerContext* context, const Empty* request, RepeatedKeyPair* response);
        virtual grpc::Status predictUserInterests(grpc::ServerContext* context, const Category* request, AlgorithmCategory* response);
        virtual grpc::Status queryNewsTrends(grpc::ServerContext* context, const Empty* request, CategoryDistribution* response);
        virtual grpc::Status queryUserInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response);
        virtual grpc::Status queryUserCurrentInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response);
        virtual grpc::Status predictMixUserInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response);
        
      protected:
        grpc::Status failed_status_glue(const Status& status);

      private:
        ContentBased* content_based_;
    };
  } // namespace recmd
} // namespace souyue
#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GRPC_H

