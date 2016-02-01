#include "service/service_grpc.h"
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    ServiceGrpc::ServiceGrpc(ContentBased* content_based)
      : content_based_(content_based)
    {
    }

    ServiceGrpc::~ServiceGrpc()
    {
    }

    grpc::Status ServiceGrpc::failed_status_glue(const Status& status)
    {
      if (status.isNotFound()) {
        return grpc::Status(grpc::NOT_FOUND, status.toString());
      } else if (status.isCorruption()) {
        return grpc::Status(grpc::INTERNAL, status.toString());
      } else {
        return grpc::Status(grpc::UNKNOWN, status.toString());
      }
    }

    grpc::Status ServiceGrpc::queryCategory(grpc::ServerContext* context, const Empty* request, RepeatedKeyPair* response)
    {
      Status status = content_based_->queryCategory(*response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::marshalUserInterests(grpc::ServerContext* context, const Category* request, AlgorithmCategory* response)
    {
      Status status = content_based_->marshalUserInterests(*request, *response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::predictUserInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response)
    {
      Status status = content_based_->predictUserInterests(request->user_id(), *response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::queryNewsTrends(grpc::ServerContext* context, const Empty* request, CategoryDistribution* response)
    {
      Status status = content_based_->queryNewsTrends(*response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::queryCurrentTrends(grpc::ServerContext* context, const Empty* request, CategoryDistribution* response)
    {
      Status status = content_based_->queryCurrentTrends(*response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::queryUserInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response)
    {
      Status status = content_based_->queryUserInterests(*request, *response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }

    grpc::Status ServiceGrpc::queryUserCurrentInterests(grpc::ServerContext* context, const Category* request, CategoryDistribution* response)
    {
      Status status = content_based_->queryUserCurrentInterests(*request, *response);
      if (status.ok()) {
        return grpc::Status::OK;
      }
      LOG(ERROR)<<status.toString()<<", from="<<context->peer();

      return failed_status_glue(status);
    }
  } // namespace news
} // namespace rsys
