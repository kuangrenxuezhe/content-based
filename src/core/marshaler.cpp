#include "core/marshaler.h"
#include <algorithm>

namespace souyue {
  namespace recmd {
    int Marshaler::selectInteresting(const vector_pair_t& interests, const vector_int_t& flags, vector_int_t& mask)
    {
      double tw = 0;
      char total_flags[100];

      for (size_t i=0; i<flags.size(); ++i) {
        total_flags[i] = flags[i];
        if (total_flags[i] == 1)
          tw += interests[i].second;
      }

      for (size_t i=0; i<mask.size(); ++i) {
        if (mask[i]>=0 && total_flags[mask[i]] == 0) {
          total_flags[mask[i]] = 1;
          tw += interests[mask[i]].second;
        }
      }
      tw = 1 - tw;

      int r = rand()%100;
      double v = 0;

      for (size_t i=0; i<interests.size(); ++i) {
        if (total_flags[i] == 1) 
          continue;
        v += interests[i].second/tw;
        if (r < v*100)
          return i;
      }

      return -1;
    }

    Status Marshaler::marshal(const vector_pair_t& trends, size_t T, vector_pair_t& results)
    {
      double pr = 0;
      size_t i=0, j=0, k=0, g=0;

      int TOPT = options_.marshaler_topt;
      int P = options_.marshaler_p;
      const int N = options_.marshaler_n;

      const double r1 = options_.marshaler_r1;
      const double r2 = options_.marshaler_r2; 

      for (i=0,pr=0; i<trends.size(); ++i) {
        pr += trends[i].second;
        if (pr > r1)
          break;
      }
      TOPT = (i+1)>TOPT?(i+1):TOPT;
      for (i=0,pr=0; i<trends.size(); ++i) {
        pr += trends[i].second;
        if (pr > r2)
          break;
      }
      P = (i+1)>P?(i+1):P;

      vector_int_t mask(N, 0);
      vector_int_t trend_flags;
      trend_flags.resize(trends.size());

      k = 0;	g = 0;
      while (results.size() < TOPT) {
        results.push_back(trends[k]);
        mask[g%N] = k;
        trend_flags[k] = 1;
        ++k;
        ++g;
      }

      while (results.size() < T) {
        int target = selectInteresting(trends,trend_flags, mask);
        if (-1 != target) {
          results.push_back(trends[target]);
          trend_flags[target] = 1;
          mask[g%N] = target;
          ++ k; ++ g;
          if (k%P == 0) 
            trend_flags.assign(trend_flags.size(), 0);
        }
      }
      return Status::OK();
    }
  } // namespace recmd
} // namespace souyue


