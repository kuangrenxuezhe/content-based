#include "core/periodic_log.h"
#include <string.h>
#include <stdlib.h>
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    static const fver_t kPeriodicLogVer(1, 0);
    static const char kWritingExt[] = ".writing";
    static const char kRecoveryExt[] = ".recovering";

    PeriodicLog::Reader::Reader(const std::string& name)
      : log_no_(0), reader_(name)
    {
    }

    PeriodicLog::Reader::~Reader()
    {
    }

    Status PeriodicLog::Reader::open()
    {
      fver_t fver;
      Status status = reader_.open(fver);
      if (!status.ok()) {
        return status;
      }
      std::string data;

      status = reader_.read(data);
      if (!status.ok()) {
        return status;
      }
      log_no_ = atoi(data.c_str());

      return Status::OK();
    }

    void PeriodicLog::Reader::close()
    {
      reader_.close();
    }

    int PeriodicLog::Reader::numb()
    {
      return log_no_;
    }

    Status PeriodicLog::Reader::read(std::string& data)
    {
      return reader_.read(data);
    }

    PeriodicLog::Iterator::Iterator(const std::string& path, const std::string& prefix, int counter)
      : counter_(counter), path_(path), prefix_(prefix)
    {
    }

    PeriodicLog::Iterator::~Iterator()
    {
    }

    void PeriodicLog::Iterator::prev()
    {
      ++counter_;
    }

    void PeriodicLog::Iterator::next()
    {
      --counter_;
    }

    std::string PeriodicLog::Iterator::filename() 
    {
      char log_name[300];

      sprintf(log_name, "%s/%s-%d.log", path_.c_str(), prefix_.c_str(), counter_);
      return std::string(log_name);
    }

    bool PeriodicLog::Iterator::isFileExist()
    {
      std::string log_name = filename();
      return access(log_name.c_str(), F_OK) ? false:true;
    }

    PeriodicLog::Reader PeriodicLog::Iterator::reader()
    {
      return Reader(filename());
    }

    PeriodicLog::PeriodicLog(const std::string& path, const std::string& prefix)
      : path_(path), prefix_(prefix), writer_(NULL)
    {
      pthread_mutex_init(&mutex_, NULL);
    }

    PeriodicLog::~PeriodicLog()
    {
      if (writer_)
        delete writer_;
      pthread_mutex_destroy(&mutex_);
    }

    PeriodicLog::Iterator PeriodicLog::getIterator()
    {
      return Iterator(path_, prefix_, last_counter_ - 1);
    }

    Status PeriodicLog::open(int counter, Visitor* visitor)
    {
      char name[300];

      sprintf(name, "%s/%s%s", path_.c_str(), prefix_.c_str(), kRecoveryExt);
      std::string recovery_name(name);

      sprintf(name, "%s/%s%s", path_.c_str(), prefix_.c_str(), kWritingExt);
      std::string writing_name(name);

      last_counter_ = counter;
      if (NULL == writer_)
        writer_ = new WALWriter(writing_name);

      if (!access(recovery_name.c_str(), F_OK)) {
        Status status = writer_->create(kPeriodicLogVer);
        if (!status.ok()) {
          return status;
        }
        return recovery(recovery_name, writer_, visitor);
      } else {
        if (access(writing_name.c_str(), F_OK)) {
          Status status = writer_->create(kPeriodicLogVer);
          if (!status.ok()) {
            return status;
          }
          char buf[100];
          sprintf(buf, "%d", last_counter_);

          return writer_->append(std::string(buf));
        } else {
          if (rename(writing_name.c_str(), recovery_name.c_str())) {
            return Status::IOError(strerror(errno), ", oldfile=", writing_name, ", newfile=", recovery_name);
          }

          Status status = writer_->create(kPeriodicLogVer);
          if (!status.ok()) {
            return status;
          }
          return recovery(recovery_name, writer_, visitor);
        }
      }     
    }

    Status PeriodicLog::recovery(const std::string& name, WALWriter* writer, Visitor* visitor)
    {
      fver_t ver;
      WALReader reader(name);

      Status status = reader.open(ver);
      if (!status.ok()) {
        return status;
      }
      bool first = true;
      std::string serialized_data;

      do {
        status = reader.read(serialized_data);
        if (!status.ok())
          break;

        if (serialized_data.length() <= 0)
          break;

        if (!status.ok()) {
          LOG(WARNING)<<status.toString();
          if (!status.isInvalidData())
            break;
        } else {
          if (first) {
            first = false;
            last_counter_ = atoi(serialized_data.c_str());
          }
          if (visitor)
            visitor->visit(serialized_data);
          status = writer->append(serialized_data);
          if (!status.ok()) {
            goto FAILED;
          }
        }
      } while (serialized_data.length() > 0);
      reader.close();

      if (::remove(name.c_str())) {
        status = Status::IOError(strerror(errno), ", file=", name);
      }
      return Status::OK();

FAILED:
      reader.close();
      return status;
    }

    Status PeriodicLog::rollover()
    {
      char log_name[300];

      sprintf(log_name, "%s/%s-%d.log", path_.c_str(), prefix_.c_str(), last_counter_);
      pthread_mutex_lock(&mutex_);
      writer_->close();
      if (rename(writer_->filename().c_str(), log_name)) {
        pthread_mutex_unlock(&mutex_);
        return Status::IOError(strerror(errno), ", oldfile=", writer_->filename(), ", newfile=", log_name);
      }

      Status status = writer_->create(kPeriodicLogVer);
      if (!status.ok()) {
        pthread_mutex_unlock(&mutex_);
        return status;
      }
      last_counter_++;
      pthread_mutex_unlock(&mutex_);
      return Status::OK();
    }

    Status PeriodicLog::write(const std::string& data)
    {
      pthread_mutex_lock(&mutex_);
      Status status = writer_->append(data);
      if (!status.ok()) {
        pthread_mutex_unlock(&mutex_);
        return status;
      }
      pthread_mutex_unlock(&mutex_);
      return Status::OK();
    }

    void PeriodicLog::close()
    {
      writer_->close();
    }
  } // namespace recmd 
} // namespace souyue
