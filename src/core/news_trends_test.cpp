#include "catch.hpp"
#include "core/news_trends.h"

using namespace souyue::recmd;

class MyNewsTrends: public NewsTrends {
  public:
    MyNewsTrends(const ModelOptions& opts)
      : NewsTrends(opts) {
    }
    virtual ~MyNewsTrends() {
    }

  public:
    virtual bool needRollover() {
      return true;
    }
};

SCENARIO("NewsTrends", "[base]") {
  GIVEN("CategoryClick") {
    ModelOptions opts;
    MyNewsTrends trends(opts);
    Status status = trends.init();
    if (!status.ok()) {
      FAIL(status.toString());
    }
   
    CategoryClick click;
    click.set_category_id(1);
    click.set_publish_time(time(NULL));
    status = trends.addClick(click);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    click.set_category_id(2);
    click.set_publish_time(time(NULL));
    status = trends.addClick(click);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    click.set_category_id(3);
    click.set_publish_time(time(NULL) - 200000);
    status = trends.addClick(click);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    status = trends.rollover();
    if (!status.ok()) {
      FAIL(status.toString());
    }

    WHEN("Train") {
      status = trends.train();
      if (!status.ok()) {
        FAIL(status.toString());
      }
      status = trends.reload();
      if (!status.ok()) {
        FAIL(status.toString());
      }
      THEN("CurrentTrends") {
        map_dist_t ctrends;

        status = trends.queryCurrentTrends(ctrends);
        if (!status.ok()) {
          FAIL(status.toString());
        }
        REQUIRE(ctrends.size() == 2);
      }
      remove("news-trends-0.log");
      remove("news-trends.dat");
      remove("news-trends.writing");
    }
  }
}
