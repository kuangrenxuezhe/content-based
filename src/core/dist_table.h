#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_DIST_TABLE_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_DIST_TABLE_H
#ifdef CPP11
#include <atomic>
#else
#include <cstdatomic>
#endif
#include "utils/chrono_expr.h"
#include "utils/table_file.h"
#include "core/periodic_log.h"
#include "core/model_options.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    class LogVisitor;
    class DistTable {
      public:
        DistTable(const std::string& path, 
            const std::string& prefix, 
            const std::string& table_name, 
            const std::string& train_timer, 
            int32_t time_window_size);
        virtual ~DistTable();

      public:
        Status init();
        Status reload();
        Status train();
        Status rollover();

      public:
        // 添加文档
        virtual Status addItem(const CategoryItem& item);
        // 添加点击日志
        virtual Status addClick(const CategoryClick& click);

      protected:
        virtual bool needRollover();

      protected:
        virtual Status reloadBefore() {
          return Status::OK();
        }
        virtual Status reloadCompleted() {
          return Status::OK();
        }
        virtual Status trainBefore() {
          return Status::OK();
        }
        virtual Status trainCompleted() {
          return Status::OK();
        }
        virtual Status recoveryClick(const CategoryClick& click) {
          return Status::OK();
        }
        virtual Status recoveryItem(const CategoryItem& item) {
          return Status::OK();
        }
        virtual Status trainClick(const CategoryClick& click) {
          return Status::OK();
        }
        virtual Status trainItem(const CategoryItem& item) {
          return Status::OK();
        }
        virtual Status dumpTable(TableFileWriter& writer) {
          return Status::OK();
        }
        virtual Status loadDistribution(const CategoryDistribution& dist) {
          return Status::OK();
        }

      protected:
        Status trainPeriodicLog();
        Status recoveryAheadLog(const std::string& data);

      private:
        std::string    work_path_;
        std::string       prefix_;
        std::string   table_name_;
        std::string  train_timer_;
        int32_t time_window_size_;

        int32_t        next_time_;
        ChronoExpr        chrono_;
        PeriodicLog periodic_log_;
        friend class LogVisitor; 
    };
  } // namespace recmd
} // namespace souyue

#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_DIST_TABLE_H
