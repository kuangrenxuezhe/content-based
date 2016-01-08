#include "catch.hpp"
#include "core/content_based.h"

using namespace souyue::recmd;
static const int kSize = 13;

void init(ModelOptions& opts) {
  opts.category_table_name = "category.dat";
  opts.user_interests_table_name = "cb-user-interests.dat";
  opts.news_trends_table_name = "cb-news-trends.dat";

  FILE* wfd = fopen("category.dat", "wb");
  if (wfd) {
    for (int i=0; i<kSize; i++)
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
  for (int i=0; i<kSize; i++) {
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
  for (int i=0; i<kSize; i++) {
    ItemTag* tag = dist1.add_distribution();
    tag->set_tag_id(i);
    tag->set_tag_power(i);
  }
  diststr = dist1.SerializeAsString();
  w1.write(diststr);
  w1.close();
}

void destroy() {
  remove("category.dat");
  remove("cb-user-interests.dat");
  remove("cb-news-trends.dat");
  remove("news-trends.writing");
  remove("user-interests.writing");
  remove("news-trends-0.log");
  remove("user-interests-0.log");
  remove("wal-item.writing");
  remove("wal-user.writing");
}

SCENARIO("ContentBased", "[base]") {
  GIVEN("Empty") {
    ModelOptions opts;

    init(opts);

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
      REQUIRE(category.key_pair_size() == kSize);

      CategoryDistribution news_trends;
      status = cb->queryNewsTrends(news_trends);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(news_trends.distribution_size() == kSize);

      Category query;
      CategoryDistribution user_dist;

      query.set_user_id(1);
      status = cb->queryUserInterests(query, user_dist);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(user_dist.distribution_size() == kSize);

      AlgorithmCategory  predict_dist;

      query.set_request_num(10);
      status = cb->marshalUserInterests(query, predict_dist);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(predict_dist.category_size() == 10);
    }
    delete cb;
    destroy();
  }
}

TEST_CASE("测试Reload") {
  ModelOptions opts;
  init(opts);

  ContentBased* cb;
  Status status = ContentBased::openModel(opts, &cb);
  if (!status.ok()) {
    FAIL(status.toString());
  }

  fver_t f(1,0);
  TableFileWriter w1("cb-user-interests.dat");
  status = w1.create(f);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  CategoryDistribution dist1;
  dist1.set_user_id(1);
  for (int i=0; i<kSize; i++) {
    ItemTag* tag = dist1.add_distribution();
    tag->set_tag_id(i);
    tag->set_tag_power(i+1);
  }
  std::string diststr = dist1.SerializeAsString();
  w1.write(diststr);
  w1.close();

  status = cb->reload();
  if (!status.ok())
    FAIL(status.toString());

  Category query;
  CategoryDistribution user_dist;

  query.set_user_id(1);
  status = cb->queryUserInterests(query, user_dist);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  REQUIRE(user_dist.distribution_size() == kSize);
  for (int i=0; i<kSize; i++) {
    const ItemTag& tag = user_dist.distribution(i);
    REQUIRE(tag.tag_id() + 1 == tag.tag_power());
  }
  delete cb;
  destroy();
}

TEST_CASE("测试Train") {
  ModelOptions opts;
  init(opts);

  ContentBased* cb;
  Status status = ContentBased::openModel(opts, &cb);
  if (!status.ok()) {
    FAIL(status.toString());
  }

  Item item;
  item.set_item_id(1);
  item.set_publish_time(time(NULL));
  ItemTag* tag = item.add_category();
  tag->set_tag_id(1);
  tag->set_tag_name("1");
  status = cb->updateItem(item);
  if (!status.ok())
    FAIL(status.toString());

  item.set_item_id(2);
  item.set_publish_time(time(NULL));
  item.clear_category();
  tag = item.add_category();
  tag->set_tag_id(2);
  tag->set_tag_name("2");
  status = cb->updateItem(item);
  if (!status.ok())
    FAIL(status.toString());

  Action action;
  action.set_user_id(10);
  action.set_item_id(1);
  action.set_action(ACTION_TYPE_CLICK);
  action.set_click_time(time(NULL) - opts.user_interests_time_window * 3600 + 1);
  status = cb->updateAction(action);
  if (!status.ok())
    FAIL(status.toString());

  Category query;
  CategoryDistribution user_dist;

  query.set_user_id(10);
  status = cb->queryUserCurrentInterests(query, user_dist);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  REQUIRE(user_dist.distribution_size() == kSize);

  int max_category = -1;
  float max_power = -1;
  for (int i=0; i<kSize; i++) {
    const ItemTag& tag = user_dist.distribution(i);
    if (tag.tag_power() > max_power) {
      max_category = tag.tag_id();
      max_power = tag.tag_power();
    }
  }
  REQUIRE(max_category == 1);

  status = cb->rollover();
  if (!status.ok()) {
    FAIL(status.toString());
  }

  action.set_user_id(10);
  action.set_item_id(2);
  action.set_action(ACTION_TYPE_CLICK);
  action.set_click_time(time(NULL));
  status = cb->updateAction(action);
  if (!status.ok())
    FAIL(status.toString());

  action.set_user_id(11);
  action.set_item_id(1);
  action.set_action(ACTION_TYPE_CLICK);
  action.set_click_time(time(NULL));
  status = cb->updateAction(action);
  if (!status.ok())
    FAIL(status.toString());

  sleep(2);
  status = cb->train();
  if (!status.ok()) {
    FAIL(status.toString());
  }

  CategoryDistribution news_trends;
  status = cb->queryNewsTrends(news_trends);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  REQUIRE(news_trends.distribution_size() == 1);

  max_category = -1, max_power = -1;
  for (int i=0; i<news_trends.distribution_size(); i++) {
    const ItemTag& tag = news_trends.distribution(i);
    if (tag.tag_power() > max_power) {
      max_category = tag.tag_id();
      max_power = tag.tag_power();
    }
    //fprintf(stdout, "%d:%f ", tag.tag_id(), tag.tag_power());
  }
  //fprintf(stdout, "\n");
  REQUIRE(max_category == 1);

  user_dist.Clear();
  status = cb->queryUserCurrentInterests(query, user_dist);
  if (!status.ok()) {
    FAIL(status.toString());
  }
  REQUIRE(user_dist.distribution_size() == kSize);

  max_category = -1, max_power = -1;
  for (int i=0; i<kSize; i++) {
    const ItemTag& tag = user_dist.distribution(i);
    if (tag.tag_power() > max_power) {
      max_category = tag.tag_id();
      max_power = tag.tag_power();
    }
    //fprintf(stdout, "%d:%f ", tag.tag_id(), tag.tag_power());
  }
  //fprintf(stdout, "\n");
  REQUIRE(max_category == 2);

  delete cb;
  destroy();
}
