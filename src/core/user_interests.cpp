#include "core/user_interests.h"
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    static const char* kClickLog = "click";
    static const uint64_t kInvalidUserId = -1;

    UserInterests::UserInterests(const ModelOptions& opts)
      : options_(opts)
      , DistTable(opts.work_path, opts.user_interests_log_prefix, opts.user_interests_table_name, opts.user_interests_reload_timer, 0)
    {
      struct tm ctm;
      time_t ctime = time(NULL);

      localtime_r(&ctime, &ctm);
      ctm.tm_hour = 0;
      ctm.tm_min = 0;
      ctm.tm_sec = 0;
      current_day_ = mktime(&ctm);

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

    Status UserInterests::queryUserInterests(uint64_t user_id, vector_pair_t& trends)
    {
      map_user_dist_t* dist = user_dist_;
      map_user_dist_t::iterator user_iter = dist->find(user_id);

      trends.clear();
      if (user_iter == dist->end()) {
        return Status::NotFound("user_id=", user_id);
      }

      map_dist_t::iterator iter = user_iter->second.begin();
      for (; iter != user_iter->second.end(); ++iter) {
        trends.push_back(std::make_pair(iter->first, iter->second));
      }
      return Status::OK();
    }

    Status UserInterests::queryCurrentUserInterests(uint64_t user_id, vector_pair_t& trends)
    {
      map_dist_t::iterator iter;
      double prior_total = 0.0, user_total = 0.0;

      trends.clear();
      pthread_mutex_lock(&mutex_);
      map_user_dist_t::iterator user_iter = current_user_dist_->find(user_id);
      if (user_iter == current_user_dist_->end()) {
        pthread_mutex_unlock(&mutex_);
        return Status::NotFound("user_id=", user_id);
      }
      for (iter = user_iter->second.begin(); iter != user_iter->second.end(); ++iter) {
        user_total += iter->second;
      }
      for (iter = current_dist_->begin(); iter != current_dist_->end(); ++iter) {
        prior_total += iter->second;
      }

      if (user_total <= 0.0)
        user_total = 1.0f;
      if (prior_total <= 0.0)
        prior_total = 1.0f;

      //fprintf(stdout, "user_total=%f, prior_total=%f\n", user_total, prior_total);
      for (iter = user_iter->second.begin(); iter != user_iter->second.end(); ++iter) {
        map_dist_t::iterator find_iter = current_dist_->find(iter->first);

        if (find_iter == current_dist_->end())
          continue;
        // pt(c|click) = D(u,c) = Nt(c)/Nt ：当前时间段，用户点击属于分类c的概率
        double pt_c_click = iter->second*1.0/user_total;
        // 先验概率 pt(c)：类c在当前时间段的分布，与用户无关
        double pt_c = find_iter->second*1.0/prior_total;
        // Nt*pt(c|click)/pt(c)：Nt为用户当前时间段点击的文档总数
        double pu = user_total * (pt_c_click/pt_c);
        // 用户自有兴趣为: (sum(Nt*pt(c|click)/pt(c))+G)/(sum(Nt)+G)
        double G = options_.user_interests_visual_clicks;
        //fprintf(stdout, "pt_c_click=%f, pt_c=%f, pu=%f, r=%f\n", pt_c_click, pt_c, pu, (pu + G)/(user_total + G));
        trends.push_back(std::make_pair(iter->first, (pu + G)/(user_total + G)));
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
      user_dist_bak_ = user_dist_.exchange(user_dist_bak_);
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

    void UserInterests::trainCategory(int32_t category_id)
    {
      pthread_mutex_lock(&mutex_);
      map_dist_t::iterator iter = current_dist_->find(category_id);
      if (iter != current_dist_->end()) {
        iter->second++;
      } else {
        current_dist_->insert(std::make_pair(category_id, 1));
      }
      pthread_mutex_unlock(&mutex_);
    }

    void UserInterests::trainUserCategory(uint64_t user_id, int32_t category_id)
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
        iter->second++;
      } else {
        user_iter->second.insert(std::make_pair(category_id, 1));
      }
      pthread_mutex_unlock(&mutex_);
    }

    Status UserInterests::recoveryClick(const CategoryClick& click)
    {
      if (click.publish_time() > current_day_) {
        if (0 == options_.user_interests_prior_prob_type) {
          trainCategory(click.category_id());
        }
        trainUserCategory(click.user_id(), click.category_id());
      }
      return Status::OK();
    }

    Status UserInterests::recoveryItem(const CategoryItem& item)
    {
      if (item.publish_time() > current_day_) {
        if (1 == options_.user_interests_prior_prob_type) {
          trainCategory(item.category_id());
        }
      }
      return Status::OK();
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
