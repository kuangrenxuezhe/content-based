#include "catch.hpp"
#include <string.h>
#include "core/model_options.h"

using namespace souyue::recmd;
TEST_CASE("ModelOptions", "[base]") {
  FILE* wfd = fopen("test.conf", "w");
  if (!wfd)
    FAIL(strerror(errno));
  // options
  fprintf(wfd, "work_path: \"mypath\"\n");
  fprintf(wfd, "item_hold_time: 11222\n");
  fprintf(wfd, "user_hold_time: 22233\n");
  fprintf(wfd, "max_table_level: 23\n");
  fprintf(wfd, "new_item_max_age: 333\n");
  fprintf(wfd, "top_item_max_age: 2323\n");

  // model_options
  fprintf(wfd, "rpc_port: 43221\n");
  fprintf(wfd, "monitor_port: 766444\n");
  fprintf(wfd, "marshaler_r1: 333333.0\n");
  fprintf(wfd, "marshaler_r2: 32222.0\n");
  fprintf(wfd, "marshaler_topt: 3333\n");
  fprintf(wfd, "marshaler_p: 333333\n");
  fprintf(wfd, "marshaler_n: 66666\n");
  fprintf(wfd, "flush_timer: \"ffff\"\n");
  fprintf(wfd, "train_timer: \"xxxxxx\"\n");
  fprintf(wfd, "rollover_timer: \"yyyyyyy\"\n");
  fprintf(wfd, "reload_timer: \"ssssss\"\n");
  fprintf(wfd, "category_table_name:  \"hhhhhhh\"\n");
  fprintf(wfd, "news_trends_table_name: \"qqqqqqq\"\n");
  fprintf(wfd, "news_trends_log_prefix: \"zzzzz\"\n");
  fprintf(wfd, "news_trends_time_window: 9999\n");
  fprintf(wfd, "user_interests_prior_prob_type: 22111\n");
  fprintf(wfd, "user_interests_visual_clicks: 77777.0\n");
  fprintf(wfd, "user_interests_table_name: \"sysz\"\n");
  fprintf(wfd, "user_interests_log_prefix: \"stffsz\"\n");
  fprintf(wfd, "user_interests_time_window: 432323\n");
  fprintf(wfd, "user_interests_mix_alpha: 222.0\n");

  fclose(wfd);

  ModelOptions opts;
  Status status = ModelOptions::fromConf("test.conf", opts);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  CHECK(opts.work_path == "mypath");
  CHECK(opts.item_hold_time == 11222);
  CHECK(opts.user_hold_time == 22233);
  CHECK(opts.max_table_level == 23);
  CHECK(opts.new_item_max_age == 333);
  CHECK(opts.top_item_max_age == 2323);

  CHECK(opts.rpc_port == 43221);
  CHECK(opts.monitor_port == 766444);
  CHECK(opts.marshaler_r1 == 333333.0);
  CHECK(opts.marshaler_r2 == 32222.0);
  CHECK(opts.marshaler_topt == 3333);
  CHECK(opts.marshaler_p == 333333);
  CHECK(opts.marshaler_n == 66666);
  CHECK(opts.flush_timer == "ffff");
  CHECK(opts.train_timer == "xxxxxx");
  CHECK(opts.rollover_timer == "yyyyyyy");
  CHECK(opts.reload_timer == "ssssss");
  CHECK(opts.category_table_name == "hhhhhhh");
  CHECK(opts.news_trends_table_name == "qqqqqqq");
  CHECK(opts.news_trends_log_prefix == "zzzzz");
  CHECK(opts.news_trends_time_window == 9999);
  CHECK(opts.user_interests_prior_prob_type == 22111);
  CHECK(opts.user_interests_visual_clicks == 77777);
  CHECK(opts.user_interests_table_name == "sysz");
  CHECK(opts.user_interests_log_prefix == "stffsz");
  CHECK(opts.user_interests_time_window == 432323);
  CHECK(opts.user_interests_mix_alpha == 222.0);

  remove("test.conf");
}

