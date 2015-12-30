#include "catch.hpp"
#include "core/content_based.h"

using namespace souyue::recmd;
SCENARIO("ContentBased", "[base]") {
  GIVEN("Empty") {
    ModelOptions opts;
    opts.category_table_name = "category.dat";
    opts.user_interests_table_name = "cb-user-interests.dat";
    opts.news_trends_table_name = "cb-news-trends.dat";

    FILE* wfd = fopen("category.dat", "wb");
    if (wfd) {
      for (int i=0; i<13; i++)
        fprintf(wfd, "%d|%d\n", i, i);
      fclose(wfd);
    }
    fver_t f(1,0);
    TableFileWriter w("cb-news-trends.dat");

    Status status = w.create(f);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    CategoryDistribution dist;
    for (int i=0; i<13; i++) {
      ItemTag* tag = dist.add_distribution();
      tag->set_tag_id(i);
      tag->set_tag_power(i);
    }
    std::string diststr = dist.SerializeAsString();
    w.write(diststr);
    w.close();

    TableFileWriter w1("cb-user-interests.dat");
    status = w1.create(f);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    CategoryDistribution dist1;
    dist1.set_user_id(1);
    for (int i=0; i<13; i++) {
      ItemTag* tag = dist1.add_distribution();
      tag->set_tag_id(i);
      tag->set_tag_power(i);
    }
    diststr = dist1.SerializeAsString();
    w1.write(diststr);
    w1.close();

    ContentBased* cb;
    status = ContentBased::openModel(opts, &cb);
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
      status = cb->marshalUserInterests(query, predict_dist);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(predict_dist.category_size() == 10);
    }
    remove("category.dat");
    remove("cb-user-interests.dat");
    remove("cb-news-trends.dat");
    remove("news-trends.writing");
    remove("user-interests.writing");
    remove("wal-item.writing");
    remove("wal-user.writing");
  }
}
