#include "catch.hpp"
#include "core/marshaler.h"

using namespace souyue::recmd;
inline bool trends_sorter(const pair_t& a, const pair_t& b)
{
  return a.second>b.second;
}

SCENARIO("Marshaler", "[marshal]") {
  ModelOptions opts;
  Marshaler marshel(opts);

  struct kv {
    int k;
    float v;
  } a[] = {
    {0 ,0.018780522},
    {1 ,0.035633918},
    {2 ,0.06859858},
    {3 ,0.1539548},
    {4 ,0.11288662},
    {5 ,0.024705544},
    {6 ,0.07396103},
    {7 ,0.01153883},
    {8 ,0.043940917},
    {9 ,0.030786173},
    {10, 0.015524753},
    {11, 0.012807623},
    {12, 0.02533994},
    {13, 0.37154076}
  };

  float t = 0, min = 1000000.0;
  for (int i =0; i < 14; i++) {
    if (a[i].v < min)
      min = a[i].v;
    if (i == 13)
      t += min;
    else
      t += a[i].v;
  }

  vector_pair_t vp, results;
  for (int i = 0; i < 14; i++) {
    if (a[i].k == 13)
      vp.push_back(std::make_pair(a[i].k, min/t));
    else
      vp.push_back(std::make_pair(a[i].k, a[i].v/t));
  }
  std::sort(vp.begin(), vp.end(), trends_sorter);

  for (size_t i = 0; i < vp.size(); i++) {
    fprintf(stdout, "%4d ", vp[i].first);
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < vp.size(); i++) {
    fprintf(stdout, "%.2f ", vp[i].second);
  }
  fprintf(stdout, "\n\n");

  marshel.marshal(vp, 20, results);
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%4d ", results[i].first);
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%.2f ", results[i].second);
  }
  fprintf(stdout, "\n");

  results.clear();
  marshel.marshal(vp, 20, results);
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%4d ", results[i].first);
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < 20; i++) {
    fprintf(stdout, "%.2f ", results[i].second);
  }
  fprintf(stdout, "\n");

  results.clear();
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
