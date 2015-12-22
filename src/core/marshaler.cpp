#include "core/marshaler.h"
#include <algorithm>

namespace souyue {
  namespace recmd {
    int Marshaler::selectInteresting(const vector_pair_t& trends, const vector_int_t& flags)
    {
      size_t i;
      double tw = 0;

      for (i = 0; i < trends.size(); ++i) {
        if (flags[i] == 1) 
          continue;

        tw += trends[i].second;
      }

      int r = rand()%100;
      double v;

      for (i = 0; i < trends.size(); ++i) {
        if (flags[i] == 1) 
          continue;

        v += trends[i].second/tw;
        if (r < v*100)
          return i;
      }
      return -1;
    }

    Status Marshaler::marshal(const vector_pair_t& trends, size_t numb, vector_pair_t& interests)
    {
      vector_int_t cand_flags, interesting_flags;

      srand(time(NULL));
      interesting_flags.resize(trends.size());

      int k = 0;
      while (interests.size() < numb && interests.size() < options_.marshaler_category_min_reserved) {
        interests.push_back(trends[k%options_.marshaler_category_min_gap]);          
        if (k > options_.marshaler_category_min_reserved-options_.marshaler_category_min_gap)
          interesting_flags[k%options_.marshaler_category_min_gap] = 1;
        ++k;
      } 

      while (interests.size() < numb) {
        int target = selectInteresting(trends, interesting_flags);
        if (target != -1) {
          interests.push_back(trends[target]);
          interesting_flags[target] = 1;
        }
        ++ k;
        if (k%options_.marshaler_category_min_gap == 0)
          interesting_flags.assign(interesting_flags.size(), 0);
      }   
      return Status::OK();
    }
  } // namespace recmd
} // namespace souyue


