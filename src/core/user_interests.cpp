#include "core/user_interests.h"
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    static const char* kClickLog = "click";
    static const uint64_t kInvalidUserId = -1;
    static const int32_t kSecondsPerHour = 3600;

    UserInterests::UserInterests(const ModelOptions& opts)
      : options_(opts)
      , DistTable(opts.work_path, opts.user_interests_log_prefix, opts.user_interests_table_name, opts.user_interests_time_window)
    {
      time_t ctime = time(NULL);
      current_day_ = ctime - opts.user_interests_time_window*kSecondsPerHour;

      current_dist_ = new map_dist_t();
      current_user_dist_ = new map_user_dist_t();
      current_user_dist_->set_empty_key(0);
      current_user_dist_->set_deleted_key(-1);
      user_dist_bak_ = new map_user_dist_t();
      user_dist_bak_->set_empty_key(0);
      user_dist_bak_->set_deleted_key(-1);

      map_user_dist_t* user_dist = new map_user_dist_t();
      user_dist->set_empty_key(0);
      user_dist->set_deleted_key(-1);
      user_dist_ = user_dist;

      pthread_mutex_init(&mutex_, NULL);
    }


    UserInterests::~UserInterests()
    {
      if (current_dist_)
        delete current_dist_;
      if (current_user_dist_)
        delete current_user_dist_;
      if (user_dist_bak_)
        delete user_dist_bak_;
      map_user_dist_t* user_dist = user_dist_;
      if (user_dist)
        delete user_dist;
    }

    Status UserInterests::init()
    {
      Status status = DistTable::init();
      if (!status.ok()) {
        return status;
      }

      // 重新训练历史数据
      return train();
    }

    Status UserInterests::train()
    {
      Status status = trainBefore();
      if (!status.ok()) {
        return status;
      }

      status = trainPeriodicLog();
      if (!status.ok()) {
        return status;
      }

      return trainCompleted();
    }

    Status UserInterests::queryCurrentTrends(map_dist_t& trends)
    {
      map_dist_t* map_dist = current_dist_;
      trends.clear();
      for (map_dist_t::iterator iter = map_dist->begin();
          iter != map_dist->end(); ++iter) {
        trends.insert(std::make_pair(iter->first, iter->second));
      }
      return Status::OK();
    }

    Status UserInterests::queryUserInterests(uint64_t user_id, map_dist_t& trends)
    {
      map_user_dist_t* dist = user_dist_;
      map_user_dist_t::iterator user_iter = dist->find(user_id);

      trends.clear();
      if (user_iter == dist->end()) {
        return Status::NotFound("user_id=", user_id);
      }

      map_dist_t::iterator iter = user_iter->second.begin();
      for (; iter != user_iter->second.end(); ++iter) {
        trends.insert(std::make_pair(iter->first, iter->second));
      }
      return Status::OK();
    }

    Status UserInterests::queryCategoryWeight(uint64_t user_id, int32_t category_id, float& weight)
    {
      map_user_dist_t* dist = user_dist_;
      map_user_dist_t::iterator user_iter = dist->find(user_id);

      if (user_iter == dist->end()) {
        return Status::NotFound("user_id=", user_id);
      }

      map_dist_t::iterator iter = user_iter->second.find(category_id);
      if (iter != user_iter->second.end()) {
        return Status::NotFound("user_id=", user_id, ", category_id=", category_id);
      }
      weight = iter->second;

      return Status::OK();
    }

    Status UserInterests::queryUserCurrentInterests(uint64_t user_id, map_category_t& map_category, map_dist_t& trends)
    {
      map_dist_t::iterator iter;
      double prior_total = 0.0, user_total = 0.0;

      trends.clear();
      pthread_mutex_lock(&mutex_);
      map_user_dist_t::iterator iter_user = current_user_dist_->find(user_id);
      if (iter_user == current_user_dist_->end()) {
        pthread_mutex_unlock(&mutex_);
        return Status::NotFound("user_id=", user_id);
      }
      for (iter = iter_user->second.begin(); iter != iter_user->second.end(); ++iter) {
        user_total += iter->second;
      }
      for (iter = current_dist_->begin(); iter != current_dist_->end(); ++iter) {
        prior_total += iter->second;
      }

      if (user_total <= 0.0)
        user_total = 1.0f;
      if (prior_total <= 0.0)
        prior_total = 1.0f;

      double G = options_.user_interests_visual_clicks;
      //fprintf(stdout, "user_total=%f, prior_total=%f\n", user_total, prior_total);
      map_category_t::iterator iter_cate = map_category.begin();
      for (; iter_cate != map_category.end(); ++iter_cate) {
        map_dist_t::iterator iter_curr = current_dist_->find(iter_cate->first);
        if (iter_curr == current_dist_->end()) {
          trends.insert(std::make_pair(iter_cate->first, G*1.0/(user_total + G)));
        } else {
          map_dist_t::iterator iter_iter_user = iter_user->second.find(iter_cate->first);
          if (iter_iter_user == iter_user->second.end()) {
            trends.insert(std::make_pair(iter_cate->first, G*1.0/(user_total + G)));
          } else {
            // pt(c|click) = D(u,c) = Nt(c)/Nt ：当前时间段，用户点击属于分类c的概率
            double pt_c_click = iter_iter_user->second*1.0/user_total;
            // 先验概率 pt(c)：类c在当前时间段的分布，与用户无关
            double pt_c = iter_curr->second*1.0/prior_total;
            // Nt*pt(c|click)/pt(c)：Nt为用户当前时间段点击的文档总数
            double pu = user_total * (pt_c_click/pt_c);
            // 用户自有兴趣为: (sum(Nt*pt(c|click)/pt(c))+G)/(sum(Nt)+G)
            //fprintf(stdout, "pt_c_click=%f, pt_c=%f, pu=%f, r=%f\n", pt_c_click, pt_c, pu, (pu + G)/(user_total + G));
            trends.insert(std::make_pair(iter_cate->first, (pu + G)/(user_total + G)));
          }
        }
      }
      pthread_mutex_unlock(&mutex_);

      return Status::OK();
    }

    Status UserInterests::reloadBefore()
    {
      user_dist_bak_->clear();
      return Status::OK();
    }

    Status UserInterests::reloadCompleted()
    {
      map_user_dist_t* tmp = user_dist_;

      user_dist_ = user_dist_bak_;
      user_dist_bak_ = tmp;
      return Status::OK();
    }

    Status UserInterests::loadDistribution(const CategoryDistribution& dist)
    {
      map_dist_t map_dist;

      for (int i = 0; i < dist.distribution_size(); ++i) {
        const ItemTag& tag = dist.distribution(i);
        map_dist.insert(std::make_pair(tag.tag_id(), tag.tag_power()));
      }
      user_dist_bak_->insert(std::make_pair(dist.user_id(), map_dist));

      return Status::OK();
    }

    Status UserInterests::trainBefore()
    {
      // 初始化是调用
      current_dist_->clear();
      current_user_dist_->clear();
      return Status::OK();
    }

    Status UserInterests::eliminateBefore()
    {
      current_day_ = time(NULL) - options_.user_interests_time_window * kSecondsPerHour;
      return Status::OK();
    }

    bool UserInterests::needEliminate(int last_counter, int counter)
    {
      return ((last_counter-counter) < options_.user_interests_time_window) ? false:true;
    }

    Status UserInterests::eliminateClick(const CategoryClick& click)
    {
      if (click.publish_time() <= current_day_) {
        if (0 == options_.user_interests_prior_prob_type) {
          trainCategory(click.category_id(), -1);
        }
        trainUserCategory(click.user_id(), click.category_id(), -1);
      }
      return Status::OK();
    }

    Status UserInterests::eliminateItem(const CategoryItem& item)
    {
      if (item.publish_time() <= current_day_) {
        if (1 == options_.user_interests_prior_prob_type) {
          trainCategory(item.category_id(), -1);
        }
      }
      return Status::OK();
    }

    Status UserInterests::trainClick(const CategoryClick& click)
    {
      if (click.publish_time() > current_day_) {
        if (0 == options_.user_interests_prior_prob_type) {
          trainCategory(click.category_id(), 1);
        }
        trainUserCategory(click.user_id(), click.category_id(), 1);
      }
      return Status::OK();
    }

    Status UserInterests::trainItem(const CategoryItem& item)
    {
      if (item.publish_time() > current_day_) {
        if (1 == options_.user_interests_prior_prob_type) {
          trainCategory(item.category_id(), 1);
        }
      }
      return Status::OK();
    }

    void UserInterests::trainCategory(int32_t category_id, int count)
    {
      pthread_mutex_lock(&mutex_);
      map_dist_t::iterator iter = current_dist_->find(category_id);
      if (iter != current_dist_->end()) {
        iter->second += count;
      } else {
        current_dist_->insert(std::make_pair(category_id, count));
      }
      pthread_mutex_unlock(&mutex_);
    }

    void UserInterests::trainUserCategory(uint64_t user_id, int32_t category_id, int count)
    {
      pthread_mutex_lock(&mutex_);
      map_user_dist_t::iterator user_iter = current_user_dist_->find(user_id);
      if (user_iter == current_user_dist_->end()) {
        std::pair<map_user_dist_t::iterator, bool> rv = 
          current_user_dist_->insert(std::make_pair(user_id, map_dist_t()));
        user_iter = rv.first;
      }
      map_dist_t::iterator iter = user_iter->second.find(category_id);
      if (iter != user_iter->second.end()) {
        iter->second += count;
        if (iter->second < 0) 
          iter->second = 0;
      } else {
        if (count < 0) 
          count = 0;
        user_iter->second.insert(std::make_pair(category_id, count));
      }
      pthread_mutex_unlock(&mutex_);
    }

    Status UserInterests::recoveryClick(const CategoryClick& click)
    {
      return trainClick(click);
    }

    Status UserInterests::recoveryItem(const CategoryItem& item)
    {
      return trainItem(item);
    }

    Status UserInterests::addItem(const CategoryItem& item)
    {
      recoveryItem(item);
      return DistTable::addItem(item);
    }

    // 添加点击日志
    Status UserInterests::addClick(const CategoryClick& click)
    {
      recoveryClick(click);
      return DistTable::addClick(click);
    }
   } // namespace recmd
} // namespace souyue
