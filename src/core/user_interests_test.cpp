#include "catch.hpp"
#include "core/user_interests.h"

using namespace souyue::recmd;

void createNewsTrend(const char* filename)
{
  TableFileWriter table(filename);
  fver_t fver(1,0);
  Status status = table.create(fver);
  if (!status.ok()) {
    fprintf(stderr, "%s\n", status.toString().c_str());
  } else {
    CategoryDistribution dist;
    ItemTag* tag = dist.add_distribution();
    tag->set_tag_id(1);
    tag->set_tag_power(1.0);
    tag = dist.add_distribution();
    tag->set_tag_id(2);
    tag->set_tag_power(2.0);
    std::string data = dist.SerializeAsString();
    status = table.write(data);
    if (!status.ok()) {
      fprintf(stderr, "%s\n", status.toString().c_str());
    }
    table.close();
  }
}

void createUserInterests(const char* filename)
{
  TableFileWriter table(filename);
  fver_t fver(1,0);
  Status status = table.create(fver);
  if (!status.ok()) {
    fprintf(stderr, "%s\n", status.toString().c_str());
  } else {
    {
      CategoryDistribution dist;
      ItemTag* tag = dist.add_distribution();
      dist.set_user_id(1);
      tag->set_tag_id(1);
      tag->set_tag_power(1.0);
      tag = dist.add_distribution();
      tag->set_tag_id(2);
      tag->set_tag_power(2.0);
      tag = dist.add_distribution();
      tag->set_tag_id(3);
      tag->set_tag_power(3.0);
      std::string data = dist.SerializeAsString();
      status = table.write(data);
      if (!status.ok()) {
        fprintf(stderr, "%s\n", status.toString().c_str());
      }
    }
    {
      CategoryDistribution dist;
      ItemTag* tag = dist.add_distribution();
      dist.set_user_id(2);
      tag->set_tag_id(1);
      tag->set_tag_power(1.0);
      tag = dist.add_distribution();
      tag->set_tag_id(2);
      tag->set_tag_power(2.0);
      std::string data = dist.SerializeAsString();
      status = table.write(data);
      if (!status.ok()) {
        fprintf(stderr, "%s\n", status.toString().c_str());
      }
    }
    table.close();
  }
}

SCENARIO("UserInterests", "[base]") {
  GIVEN("Empty interests table") {
    ModelOptions opts;
    UserInterests interests(opts);
    Status status = interests.init();
    if (!status.ok()) {
      FAIL(status.toString());
    }

    WHEN("Add click") {
      CategoryClick click;
      click.set_user_id(1);
      click.set_category_id(10);
      click.set_publish_time(time(NULL));
      status = interests.addClick(click);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      click.set_user_id(1);
      click.set_category_id(11);
      click.set_publish_time(time(NULL));
      status = interests.addClick(click);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      for (int i = 0; i < 10; i++) {
        click.set_user_id(1);
        click.set_category_id(10);
        click.set_publish_time(time(NULL));
        status = interests.addClick(click);
        if (!status.ok()) {
          FAIL(status.toString());
        }
      }
      THEN("Current interests") {
        vector_pair_t trends;
        status = interests.queryCurrentUserInterests(1, trends);
        if (!status.ok()) {
          FAIL(status.toString());
        }
        REQUIRE(trends.size() == 2);
        for (int i = 0; i < 2; i++) {
          //fprintf(stdout, "%d:%.2f\n", trends[i].first, trends[i].second);
        }
        remove("user-interests.writing");
      }
    }
  }

  GIVEN("Empty interests table") {
    ModelOptions opts;
    UserInterests interests(opts);
    Status status = interests.init();
    if (!status.ok()) {
      FAIL(status.toString());
    }

    WHEN("Reload") {
      CategoryClick click;
      click.set_user_id(1);
      click.set_category_id(10);
      click.set_publish_time(time(NULL));
      status = interests.addClick(click);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      click.set_user_id(1);
      click.set_category_id(11);
      click.set_publish_time(time(NULL));
      status = interests.addClick(click);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      for (int i = 0; i < 10; i++) {
        click.set_user_id(1);
        click.set_category_id(10);
        click.set_publish_time(time(NULL));
        status = interests.addClick(click);
        if (!status.ok()) {
          FAIL(status.toString());
        }
      }
      //createNewsTrend("news-trends.dat");
      createUserInterests("user-interests.dat");

      status = interests.reload();
      if (!status.ok()) {
        FAIL(status.toString());
      }

      THEN("User interests") {
        vector_pair_t trends;
        status = interests.queryCurrentUserInterests(1, trends);
        if (!status.ok()) {
          FAIL(status.toString());
        }
        REQUIRE(trends.size() == 2);
        for (int i = 0; i < 2; i++) {
          //fprintf(stdout, "%d:%.2f\n", trends[i].first, trends[i].second);
        }
        vector_pair_t user_trends;

        status = interests.queryUserInterests(1, user_trends);
        if (!status.ok()) {
          FAIL(status.toString());
        }
        REQUIRE(user_trends.size() == 3);
      }
      remove("user-interests.dat");
      remove("user-interests.writing");
    }
  }
}
