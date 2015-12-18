#include "catch.hpp"
#include "core/periodic_log.h"

using namespace souyue::recmd;
SCENARIO("PeriodicLog", "[base]") {
  GIVEN("Empty PeriodicLog") {
    PeriodicLog log(".", "periodic");

   WHEN("Open") {
     Status status = log.open(0);
     THEN("Success") {
       if (!status.ok()) {
         FAIL(status.toString());
       }
       log.close();
     }
     remove("periodic.writing");
   }
    WHEN("Write") {
      Status status = log.open(0);
      THEN("Success") {
        if (!status.ok()) {
          FAIL(status.toString());
        }
        status = log.write("1");
        if (!status.ok()) {
          FAIL(status.toString());
        }
        log.close();
      }
    }
    WHEN("Rollover") {
      Status status = log.open(0);
      if (!status.ok()) {
        FAIL(status.toString());
      }
      status = log.rollover();
      THEN("Success") {
        if (!status.ok()) {
          FAIL(status.toString());
        }

        PeriodicLog::Iterator iter = log.getIterator();
        REQUIRE(iter.isFileExist());

        PeriodicLog::Reader reader = iter.reader();
        fver_t fver;

        status = reader.open();
        if (!status.ok()) {
          FAIL(status.toString());
        }
        std::string data;

        status = reader.read(data);
        if (!status.ok()) {
          FAIL(status.toString());
        }
        REQUIRE(data == "1");
        REQUIRE(reader.numb() == 0);

        log.close();
      }
      remove("periodic-0.log");
      remove("periodic.writing");
    }
  }
}
