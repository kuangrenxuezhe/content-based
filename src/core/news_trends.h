#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_NEWS_TRENDS_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_NEWS_TRENDS_H
#ifdef CPP11
#include <atomic>
#else
#include <cstdatomic>
#endif
#include <map>
#include <vector>
#include "core/core_type.h"
#include "utils/table_file.h"
#include "core/model_options.h"
#include "core/dist_table.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    class NewsTrends: public DistTable {
      public:
        NewsTrends(const ModelOptions& opts);
        ~NewsTrends();

      public:
        // 获取当前趋势, 按照趋势从大到小排序
        // pair->first: category_id
        // pair->second: probability
        Status queryCurrentTrends(vector_pair_t& trends);

      protected:
        virtual Status reloadBefore();
        virtual Status reloadCompleted();

        virtual Status trainBefore();
        virtual Status trainCompeleted();

        virtual Status trainClick(const CategoryClick& click);

        virtual Status dumpTable(TableFileWriter& writer);
        virtual Status loadDistribution(const CategoryDistribution& dist);

      private:
        ModelOptions                  options_;
        int32_t                 click_expired_;
        map_dist_t*         map_dist_training_;
        std::atomic<map_dist_t*>     map_dist_;
    };
  } // namespace recmd
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_NEWS_TRENDS_H
