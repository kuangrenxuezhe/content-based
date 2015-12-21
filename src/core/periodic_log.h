#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_PERIODIC_LOG_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_PERIODIC_LOG_H
#include "utils/wal.h"
#include "utils/chrono_expr.h"

namespace souyue {
  namespace recmd {
    class PeriodicLog {
      public:
        class Visitor {
          public:
            Visitor() { }
            ~Visitor() { }

          public:
            virtual void visit(const std::string& data) = 0;
        };
        class Iterator;
        class Reader {
          public:
           ~Reader();

          public:
            Status open();
            void close();

            int numb();
            Status read(std::string& data);

          private:
            Reader(const std::string& name);
            friend class Iterator;
 
          private:
            int       log_no_;
            WALReader reader_;
        };

        class Iterator {
          public:
            ~Iterator();

          public:
            void prev();
            void next();

            Reader reader();
            bool isFileExist();

            int counter();
            int last_counter();
            std::string filename();

          private:
            Iterator(const std::string& path, const std::string& prefix, int counter);
            friend class PeriodicLog;

          private:
            int   last_counter_;
            int        counter_;
            std::string   path_;
            std::string prefix_;
        };

      public:
        PeriodicLog(const std::string& path, const std::string& prefix);
        ~PeriodicLog();

      public:
        // 若prefix.writing存在则会覆盖last_counter
        // 否则使用counter作为last_counter
        Status open(int counter, Visitor* visitor = NULL);
        // log滚存计数
        Status rollover();
        Status write(const std::string& data);
        void close();

      public:
        Iterator getIterator();

      protected:
        Status recovery(const std::string& name, WALWriter* writer, Visitor* visitor);

      private:
        int      last_counter_;
        std::string      path_; // 文件保存路径
        std::string    prefix_; // log名称前缀
        WALWriter*     writer_; // log writer
        pthread_mutex_t mutex_; // 互斥器
    };
  } // namespace recmd
} // namespace souyue
#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_PERIODIC_LOG_H

