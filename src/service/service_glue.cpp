#include "service/service_glue.h"
#include "utils/status.h"
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    ServiceGlue::ServiceGlue(ContentBased* content_based)
      : content_based_(content_based)
    {
    }

    ServiceGlue::~ServiceGlue()
    {
    }

    var_4 ServiceGlue::init_module(var_vd* config_info) {
      const ModelOptions& options = content_based_->options();
      Status status = chrono_flush_.parse(options.flush_timer);
      if (!status.ok()) {
        LOG(ERROR) << status.toString();
        return -1;
      }
      next_flush_time_ = chrono_flush_.next(time(NULL));

      status = chrono_train_.parse(options.news_trends_train_timer);
      if (!status.ok()) {
        LOG(ERROR) << status.toString();
        return -1;
      }
      next_train_time_ = chrono_train_.next(time(NULL));

      return 0;
    }

    var_4 ServiceGlue::update_action(Action& action)
    {
      Status status = content_based_->updateAction(action);
      if (status.ok()) {
        return 0;
      }
      LOG(ERROR)<<status.toString();

      if (status.isCorruption() || status.isIOError())
        return -1;
      return 0;
    }

    var_4 ServiceGlue::update_item(const Item& item)
    {
      Status status = content_based_->updateItem(item);
      if (status.ok()) {
        return 0;
      }
      LOG(ERROR)<<status.toString();

      if (status.isCorruption() || status.isIOError())
        return -1;
      return 0;
    }

    var_4 ServiceGlue::query_algorithm(const CandidateSetBase& csb, AlgorithmPower* ap)
    {
      Status status = content_based_->queryCategoryWeight(csb, *ap);
      if (status.ok()) {
        return 0;
      }
      LOG(ERROR)<<status.toString();

      if (status.isCorruption() || status.isIOError())
        return -1;
      return 0;
    }

    var_4 ServiceGlue::query_user_category(const Category& category, AlgorithmCategory* ac)
    {
      Status status = content_based_->marshalUserInterests(category, *ac);
      if (status.ok()) {
        return 0;
      }
      LOG(ERROR)<<status.toString();

      if (status.isCorruption() || status.isIOError())
        return -1;
      return 0;
    }

    var_4 ServiceGlue::persistent_library()
    {
      int32_t ctime = time(NULL);
      if (next_flush_time_ < ctime) {
        Status status = content_based_->flush();
        if (!status.ok()) {
          LOG(FATAL) << status.toString();
          return -1;
        }
        next_flush_time_ = chrono_flush_.next(next_flush_time_);
      }
      if (next_train_time_ < ctime) {
        Status status = content_based_->train();
        if (!status.ok()) {
          LOG(FATAL) << status.toString();
          return -1;
        }
        status = content_based_->reload();
        if (!status.ok()) {
          LOG(FATAL) << status.toString();
          return -1;
        }
        next_train_time_ = chrono_train_.next(next_flush_time_);
      }
      return 0;
    }
  } // namespace recmd 
} // namespace souyue 

