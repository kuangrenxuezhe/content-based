#ifndef SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
#define SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
#include <vector>
#include "utils/status.h"
#include "core/core_type.h"
#include "core/model_options.h"

namespace souyue {
  namespace recmd {
    class Marshaler {
      public:
        typedef std::vector<int> vector_int_t;

      public:
        Marshaler(const ModelOptions& opts)
          : options_(opts) { }
        ~Marshaler() { }

      public:
        // 摇号前trends的权重必须归一
        Status marshal(const vector_pair_t& trends, size_t numb, vector_pair_t& interests);

      private:
        int selectInteresting(const vector_pair_t& trends, const vector_int_t& flags, vector_int_t& mask);

      private:
        ModelOptions options_;
    };
  } // namespace recmd
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
