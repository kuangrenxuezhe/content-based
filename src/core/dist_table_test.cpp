#include "catch.hpp"
#include "core/dist_table.h"

using namespace souyue::recmd;
class TestDistTable: public DistTable {
  public:
    TestDistTable(const std::string& timer)
      : DistTable(".", "prefix", "distab", timer, 1) {
    }
    virtual ~TestDistTable() {
    }

  public:
    virtual bool needRollover() {
      return true;
    }
    virtual Status dumpTable(TableFileWriter& writer) {
      CategoryDistribution dist;
      std::map<uint64_t, int>::iterator iter = map_id_.begin();
      for (; iter != map_id_.end(); ++iter) {
        ItemTag* tag = dist.add_distribution();
        tag->set_tag_id(iter->first);
        tag->set_tag_power(iter->second);
      }
      std::string data = dist.SerializeAsString();
      return writer.write(data);
    }
    virtual Status trainClick(const CategoryClick& click) {
      std::map<uint64_t, int>::iterator iter = map_id_.find(click.user_id());
      if (iter == map_id_.end()) {
        map_id_.insert(std::make_pair(click.user_id(), 1));
      } else {
        iter->second++;
      }
      return Status::OK();
    }
    virtual Status trainItem(const CategoryItem& item) {
      std::map<uint64_t, int>::iterator iter = map_id_.find(item.item_id());
      if (iter == map_id_.end()) {
        map_id_.insert(std::make_pair(item.item_id(), 1));
      } else {
        iter->second++;
      }
      return Status::OK();
    }
    virtual Status loadDistribution(const CategoryDistribution& dist) {
      map_id_.clear();
      for (int i = 0; i < dist.distribution_size(); ++i) {
        const ItemTag& tag = dist.distribution(i);
        map_id_.insert(std::make_pair(tag.tag_id(), int(tag.tag_power())));
      }
      return Status::OK();
    }
    std::map<uint64_t, int> map_id_;
};

SCENARIO("DistTable", "[base]") {
  GIVEN("Empty table") {
    TestDistTable table("1/hour");
    Status status = table.init();
    if (!status.ok()) {
      FAIL(status.toString());
    }
    WHEN("Reload") {
      status = table.reload();
      THEN("Failed") {
        REQUIRE(!status.ok());
      }
      remove("prefix.writing");
    }
  }

  GIVEN("Rollover log") {
    TestDistTable  table("1/hour");
    Status status = table.init();
    if (!status.ok()) {
      FAIL(status.toString());
    }

    CategoryClick click;
    click.set_user_id(1);
    status = table.addClick(click);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    CategoryItem item;
    item.set_item_id(2);
    status = table.addItem(item);
    if (!status.ok()) {
      FAIL(status.toString());
    }
    status = table.rollover();
    if (!status.ok()) {
      FAIL(status.toString());
    }
    WHEN("Train") {
      status = table.train();
      if (!status.ok()) {
        FAIL(status.toString());
      }
      std::map<uint64_t, int> map_bak = table.map_id_;
      status = table.reload();
      if (!status.ok()) {
        FAIL(status.toString());
      }
      REQUIRE(map_bak.size() == table.map_id_.size());
    }
    remove("distab");
    remove("prefix-0.log");
    remove("prefix.writing");
  }
}
