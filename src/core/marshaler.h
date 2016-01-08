#ifndef SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
#define SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
#include <vector>
#ifdef CPP11
#include <random>
#endif
#include "utils/status.h"
#include "core/core_type.h"
#include "core/model_options.h"

namespace souyue {
  namespace recmd {
#ifdef CPP11
    using std::mt19937;
    using std::random_device;
#endif
    class Marshaler {
      public:
        typedef std::vector<int> vector_int_t;

      public:
        Marshaler(const ModelOptions& opts);
        ~Marshaler() { }

      public:
        // 摇号前trends的权重必须归一
        Status marshal(const vector_pair_t& trends, size_t numb, vector_pair_t& interests);

      private:
        int selectInteresting(const vector_pair_t& trends, const vector_int_t& flags, vector_int_t& mask);

      private:
        ModelOptions options_;
#ifdef CPP11
        random_device device_;
        mt19937 engine_;
        std::uniform_real_distribution<double> dist_;
#endif
    };
  } // namespace recmd
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODEL_CONTENT_BASED_MARSHALER_H
