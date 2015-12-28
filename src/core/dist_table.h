#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_DIST_TABLE_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_DIST_TABLE_H
#ifdef CPP11
#include <atomic>
#include <functional>
#else
#include <cstdatomic>
#include <tr1/functional>
#endif
#include "utils/chrono_expr.h"
#include "utils/table_file.h"
#include "core/periodic_log.h"
#include "core/model_options.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    using namespace module::protocol;
#ifdef CPP11
    using std::function;
    using std::bind;
    using namespace std::placeholders;
#else
    using std::tr1::function;
    using std::tr1::bind;
    using namespace std::tr1::placeholders;
#endif
    class LogVisitor;
    class DistTable {
      public:
        typedef function<Status (const CategoryClick&)> click_func_t;
        typedef function<Status (const CategoryItem&)> item_func_t;

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
        Status eliminate();

      public:
        // 添加文档
        virtual Status addItem(const CategoryItem& item);
        // 添加点击日志
        virtual Status addClick(const CategoryClick& click);

      protected:
        virtual bool needRollover();
        virtual bool needEliminate(int last_counter, int counter) {
          return false;
        }

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
        virtual Status eliminateBefore() {
          return Status::OK();
        }
        virtual Status eliminateCompleted() {
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
        virtual Status eliminateClick(const CategoryClick& click) {
          return Status::OK();
        }
        virtual Status eliminateItem(const CategoryItem& item) {
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
        Status eliminatePeriodicLog();

        Status recoveryAheadLog(const std::string& data);
        Status readPeriodicLog(PeriodicLog::Reader reader, click_func_t& clickf, item_func_t& itemf);

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
