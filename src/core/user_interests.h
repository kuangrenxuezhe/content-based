#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_USER_INTERESTS_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_USER_INTERESTS_H
#include "core/core_type.h"
#include "core/dist_table.h"
#include "sparsehash/dense_hash_map"

namespace souyue {
  namespace recmd {
    class UserInterests: public DistTable {
      public:
        typedef google::dense_hash_map<uint64_t, map_dist_t> map_user_dist_t;

      public:
        UserInterests(const ModelOptions& opts);
        ~UserInterests();

      public:
        Status init();
        Status train();

        // 获取当前趋势, 按照趋势从大到小排序
        // pair->first: category_id
        // pair->second: probability
        Status queryUserInterests(uint64_t user_id, map_dist_t& trends);
        Status queryCategoryWeight(uint64_t user_id, int32_t category_id, float& weight);

        Status queryCurrentTrends(map_dist_t& trends);
        Status queryUserCurrentInterests(uint64_t user_id, map_category_t& map_category, map_dist_t& trends);

      public:
        // 添加文档
        virtual Status addItem(const CategoryItem& item);
        // 添加点击日志
        virtual Status addClick(const CategoryClick& click);

      protected:
        virtual Status reloadBefore();
        virtual Status reloadCompleted();

        virtual Status trainBefore();
        virtual Status eliminateCompleted();

        virtual Status trainClick(const CategoryClick& click);
        virtual Status trainItem(const CategoryItem& item);

        virtual Status recoveryClick(const CategoryClick& click);
        virtual Status recoveryItem(const CategoryItem& item);

        virtual Status eliminateClick(const CategoryClick& click);
        virtual Status eliminateItem(const CategoryItem& item);
 
        virtual Status loadDistribution(const CategoryDistribution& dist);

      protected:
        bool needEliminate(int last_counter, int counter);
        void trainCategory(int32_t category_id, int count);
        void trainUserCategory(uint64_t user_id, int32_t category_id, int count);

      private:
        ModelOptions                 options_;
        int32_t                  current_day_;
        // 当前用户兴趣分布
        map_dist_t*             current_dist_;
        map_user_dist_t*   current_user_dist_;
        // 用户长期兴趣分布
        map_user_dist_t*       user_dist_bak_;
        map_user_dist_t*           user_dist_;
        pthread_mutex_t                mutex_;
    };
  } // namespace recmd
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_USER_INTERESTS_H

