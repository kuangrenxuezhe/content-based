#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_H

#include "utils/status.h"
#include "db/user_db.h"
#include "db/item_db.h"
#include "core/model_options.h"
#include "core/marshaler.h"
#include "core/news_trends.h"
#include "core/user_interests.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    class ContentBased {
      public:
        ContentBased(const ModelOptions& opts);
        ~ContentBased();

      public:
        const ModelOptions& options() const { return options_; }
        static Status openModel(const ModelOptions& opts, ContentBased** model_ptr);

      public:
        // 可异步方式，线程安全
        Status train();
        Status flush();
        Status reload();

      public:
        // 查询当前已有的分类信息
        Status queryCategory(RepeatedKeyPair& category);
        // 查询候选集权重
        Status queryCategoryWeight(const CandidateSetBase& csb, AlgorithmPower& power);
        // 预测用户的当前兴趣
        Status predictUserInterests(const Category& category, AlgorithmCategory& dist);

      public:
        // 用户点击更新
        Status updateAction(Action& action);
        // 添加&更新新闻数据
        Status updateItem(const Item& item);
 
      public:
       // 查询新闻趋势
        Status queryNewsTrends(CategoryDistribution& dist);
        // 查询用户历史兴趣
        Status queryUserInterests(const Category& query, CategoryDistribution& dist);
        // 查询用户当前兴趣
        Status queryUserCurrentInterests(const Category& query, CategoryDistribution& dist);
        // 预测用户的当前兴趣
        Status predictUserInterests(const Category& category, CategoryDistribution& dist);

      private:
        Status lock();
        float totalDistribution(const map_dist_t& trends);

      private:
        ModelOptions          options_;
        FileWriter         model_lock_; // 单进程锁
        UserDB*               user_db_; // 点击排重
        ItemDB*               item_db_; // 找回点击item的分类
        Marshaler*          marshaler_; // 用户兴趣分布打散
        NewsTrends*       news_trends_; // 当前新闻趋势
        UserInterests* user_interests_; // 用户兴趣分布趋势
    };
  }
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_H
