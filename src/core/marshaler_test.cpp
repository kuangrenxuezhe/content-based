#include "catch.hpp"
#include "core/marshaler.h"

using namespace souyue::recmd;
SCENARIO("Marshaler", "[base]") {
  ModelOptions opts;
  Marshaler marshel(opts);

  vector_pair_t vp, results;
  vp.push_back(std::make_pair(0, 4.59));
  vp.push_back(std::make_pair(5, 2.12));
  vp.push_back(std::make_pair(2, 2.05));
  vp.push_back(std::make_pair(9, 1.18));
  vp.push_back(std::make_pair(8, 0.78));
  vp.push_back(std::make_pair(4, 0.76));
  vp.push_back(std::make_pair(6, 0.68));
  vp.push_back(std::make_pair(3, 0.61));
  vp.push_back(std::make_pair(1, 0.47));
  vp.push_back(std::make_pair(10, 0.43));
  vp.push_back(std::make_pair(12, 0.37));
  vp.push_back(std::make_pair(11, 0.17));
  vp.push_back(std::make_pair(7, 0.01));
  float total = 0.0;
  for (size_t i = 0; i < vp.size(); ++i) {
    total += vp[i].second;
  }
  for (size_t i = 0; i < vp.size(); ++i) {
    vp[i].second = vp[i].second/total;
  }
  marshel.marshal(vp, 20, results);
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%4d ", results[i].first);
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%.2f ", results[i].second);
  }
  fprintf(stdout, "\n");
}
