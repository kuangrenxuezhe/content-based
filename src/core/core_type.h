#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_CORE_TYPE_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_CORE_TYPE_H
#include <map>
#include <set>
#include <string>
#include <vector>

namespace souyue {
  namespace recmd {
    typedef std::pair<int32_t, double>    pair_t;
    typedef std::vector<pair_t>    vector_pair_t;
    typedef std::map<int32_t, double> map_dist_t;
    typedef std::map<int32_t, std::string>  map_category_t;
  } // namespace recmd
} // namespace souyue
#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_CORE_TYPE_H

