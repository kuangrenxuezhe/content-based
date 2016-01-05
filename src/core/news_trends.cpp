#include "core/news_trends.h"
#include <algorithm>
#include "utils/table_file.h"
#include "glog/logging.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    static const fver_t kNewsTrendsVer(1, 0);
    static const int32_t kSecondsPerHour = 3600;

    inline bool trends_sorter(const pair_t& a, const pair_t& b)
    {
      return a.second>b.second;
    }

    NewsTrends::NewsTrends(const ModelOptions& opts)
      : options_(opts)
      , map_dist_training_(NULL)
      , DistTable(opts.work_path, opts.news_trends_log_prefix, opts.news_trends_table_name, opts.news_trends_time_window)
    {
      map_dist_training_ = new map_dist_t();
      map_dist_ = new map_dist_t();
    }

    NewsTrends::~NewsTrends()
    {
      map_dist_t* map_dist = map_dist_;
      if (map_dist)
        delete map_dist;
      if (map_dist_training_)
        delete map_dist_training_;
    }

    Status NewsTrends::reloadBefore()
    {
      map_dist_training_->clear();
      return Status::OK();
    }

    Status NewsTrends::reloadCompleted()
    {
      std::swap(map_dist_, map_dist_training_);
      return Status::OK();
    }

    Status NewsTrends::loadDistribution(const CategoryDistribution& distribution)
    {
      for (int i = 0; i < distribution.distribution_size(); ++i) {
        const ItemTag& tag = distribution.distribution(i);
        map_dist_training_->insert(std::make_pair(tag.tag_id(), tag.tag_power()));
      }
      return Status::OK();
    }

    Status NewsTrends::dumpTable(TableFileWriter& writer)
    {
      CategoryDistribution distribution;

      int32_t total_click = 0;
      for (map_dist_t::const_iterator iter = map_dist_training_->begin(); 
          iter != map_dist_training_->end(); ++iter) {
        total_click += iter->second;
      }
 
      if (total_click <= 0) 
        total_click = 1;

      for (map_dist_t::iterator iter = map_dist_training_->begin(); 
          iter != map_dist_training_->end(); ++iter) {
        ItemTag* tag = distribution.add_distribution();
        tag->set_tag_id(iter->first);
        iter->second = iter->second/float(total_click);
        tag->set_tag_power(iter->second);
      }
      std::string data = distribution.SerializeAsString();

      Status status = writer.write(data);
      if (!status.ok()) {
        return status;
      }

      return Status::OK();
    }

    Status NewsTrends::trainBefore()
    {
      map_dist_training_->clear();
      click_expired_ = time(NULL) - options_.news_trends_time_window*kSecondsPerHour;

      return Status::OK();
    }

    Status NewsTrends::trainCompleted()
    {
      std::swap(map_dist_, map_dist_training_);
      return Status::OK();
    }

    Status NewsTrends::trainClick(const CategoryClick& click)
    {
      map_dist_t::iterator map_iter;

      if (click.publish_time() < click_expired_)
        return Status::OK();

      map_iter = map_dist_training_->find(click.category_id());
      if (map_iter != map_dist_training_->end()) {
      map_iter->second++;
      } else {
        map_dist_training_->insert(std::make_pair(click.category_id(), 1));
      }

      return Status::OK();
    }
    
    Status NewsTrends::queryCurrentTrends(map_dist_t& trends)
    {
      map_dist_t* map_dist = map_dist_;

      trends.clear();
      for (map_dist_t::iterator iter = map_dist->begin();
          iter != map_dist->end(); ++iter) {
        trends.insert(std::make_pair(iter->first, iter->second));
      }

      return Status::OK();
    }

    Status NewsTrends::queryCategoryWeight(int32_t category_id, float& weight)
    {
      map_dist_t* map_dist = map_dist_;
      map_dist_t::iterator iter = map_dist->find(category_id);
      if (iter != map_dist->end()) {
        weight = iter->second;
        return Status::OK();
      }
      return Status::NotFound("category_id=", category_id);
    }
  } // namespace recmd
} // namespace souyue
