#include "catch.hpp"
#include "core/content_based.h"

using namespace souyue::recmd;
SCENARIO("ContentBased", "[base]") {
  GIVEN("Empty") {
    ModelOptions opts;
    opts.user_interests_table_name = "cb-user-interests.dat";
    opts.news_trends_table_name = "cb-news-trends.dat";

    ContentBased* cb;
    Status status = ContentBased::openModel(opts, &cb);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    WHEN("Query category") {
      RepeatedKeyPair category;
      status = cb->queryCategory(category);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(category.key_pair_size() == 13);

      CategoryDistribution news_trends;
      status = cb->queryNewsTrends(news_trends);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(news_trends.distribution_size() == 13);

      Category query;
      CategoryDistribution user_dist;
      
      query.set_user_id(1);
      status = cb->queryUserInterests(query, user_dist);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(user_dist.distribution_size() == 13);

      AlgorithmCategory  predict_dist;

      query.set_request_num(10);
      status = cb->predictUserInterests(query, predict_dist);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(predict_dist.category_size() == 10);
    }
  }
}
