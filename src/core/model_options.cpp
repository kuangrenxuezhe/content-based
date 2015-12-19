#include "core/model_options.h"
#include "libconfig.hh"

namespace souyue {
  namespace recmd {
    using namespace libconfig;

    ModelOptions::ModelOptions()
    {
      rpc_port = 6100;
      monitor_port = 16100;

      flush_timer = "23/day";
      category_table_name = "category.dat";

      news_trends_table_name = "news-trends.dat";
      news_trends_log_prefix = "news-trends";
      news_trends_time_window = 48;
      news_trends_train_timer = "30/hour";

      user_interests_prior_prob_type = 0;
      user_interests_visual_clicks = 10;
      user_interests_table_name = "user-interests.dat";
      user_interests_log_prefix = "user-interests";
      user_interests_reload_timer = "23/day";

      marshaler_category_min_gap = 5;
    }

    Status ModelOptions::fromConf(const std::string& conf, ModelOptions& opts)
    {
      Config parser;

      Status status = Options::fromConf(conf, opts);
      if (!status.ok()) {
        return status;
      }

      try {
        parser.readFile(conf.c_str());

        if (!parser.lookupValue("rpc_port", opts.rpc_port))
          opts.rpc_port = 6100;
        if (!parser.lookupValue("monitor_port", opts.monitor_port))
          opts.monitor_port = 16100;
        if (!parser.lookupValue("category_table_name", opts.category_table_name))
          opts.category_table_name = "category.dat";
        if (!parser.lookupValue("news_trends_table_name", opts.news_trends_table_name))
          opts.news_trends_table_name = "news-trends.dat";
        if (!parser.lookupValue("news_trends_log_prefix", opts.news_trends_log_prefix))
          opts.news_trends_log_prefix = "news-trends";
        if (!parser.lookupValue("news_trends_time_window", opts.news_trends_time_window))
          opts.news_trends_time_window = 48;
        if (!parser.lookupValue("news_trends_train_timer", opts.news_trends_train_timer))
          opts.news_trends_train_timer = "30/hour";
        if (!parser.lookupValue("user_interests_prior_prob_type", opts.user_interests_prior_prob_type))
          opts.user_interests_prior_prob_type = 0;
        if (!parser.lookupValue("user_interests_visual_clicks", opts.user_interests_visual_clicks))
          opts.user_interests_visual_clicks = 10;
        if (!parser.lookupValue("user_interests_table_name", opts.user_interests_table_name))
          opts.user_interests_table_name = "user-interests.dat";
        if (!parser.lookupValue("user_interests_log_prefix", opts.user_interests_log_prefix))
          opts.user_interests_log_prefix = "user-interests";
        if (!parser.lookupValue("user_interests_reload_timer", opts.user_interests_reload_timer))
          opts.user_interests_reload_timer = "23/day";
      }
      catch(const FileIOException &oex) {
        return Status::IOError(oex.what(), ", file=", conf);
      }
      catch(const ParseException &pex) {
        return Status::Corruption(pex.getError(), ", at ", conf, ":", pex.getLine());
      }
      return Status::OK();
    }
  } // namespace recmd
} // namespace souyue