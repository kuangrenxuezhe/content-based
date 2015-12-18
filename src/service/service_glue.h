#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GLUE_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GLUE_H

#include "utils/chrono_expr.h"
#include "core/content_based.h"
#include "framework/CF_framework_interface.h"

namespace souyue {
  namespace recmd {
    static const int32_t kPersistentLibrary = 1;
    static const int32_t kUpdateTrain = 0;
    static const int32_t kContentBasedModuleType = 1;

    class ServiceGlue: public CF_framework_interface {
      public:
        ServiceGlue(ContentBased* content_based);
        virtual ~ServiceGlue();

      public:
        virtual var_4 module_type() {
          return kContentBasedModuleType;
        }

       virtual var_4 is_persistent_library() {
          return kPersistentLibrary;
        }
        virtual var_4 is_update_train() {
          return kUpdateTrain;
        }

      public:
        virtual var_4 init_module(var_vd* config_info);

        virtual var_4 update_action(Action& action);
        virtual var_4 update_item(const Item& item);
        virtual var_4 query_user_category(const Category& category, AlgorithmCategory* ac);
    
        virtual var_4 persistent_library();

      private:
        int32_t     next_flush_time_;
        ChronoExpr     chrono_flush_;

        int32_t     next_train_time_;
        ChronoExpr     chrono_train_;

        ContentBased* content_based_;
    };
  }; // namespace recmd
}; // namespace souyue

#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_SERVICE_GLUE_H

