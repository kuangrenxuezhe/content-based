#include "core/dist_table.h"
#include "utils/util.h"
#include "utils/table_file.h"
#include "glog/logging.h"
#include "proto/supplement.pb.h"

namespace souyue {
  namespace recmd {
    static fver_t     kDistTableVer(1, 0);
    static const char kLogTypeClick = 'C';
    static const char kLogTypeItem  = 'I';

    class LogVisitor: public PeriodicLog::Visitor {
      public:
        LogVisitor(DistTable* table): table_(table) {
        }
        virtual ~LogVisitor() { }
        virtual void visit(const std::string& data) {
          table_->recoveryAheadLog(data);
        }

      private:
        DistTable* table_;
    };

    DistTable::DistTable(const std::string& path, 
        const std::string& prefix, 
        const std::string& table_name, 
        int32_t time_window_size)
      : work_path_(path),
        prefix_(prefix),
        table_name_(table_name), 
        time_window_size_(time_window_size), 
        periodic_log_(path, prefix)
    {
    }

    DistTable::~DistTable()
    {
    }

    Status DistTable::init()
    {
      LogVisitor visitor(this);

      Status status = periodic_log_.open(0, &visitor);
      if (!status.ok()) {
        return status;
      }
      return Status::OK();
    }

    Status DistTable::reload()
    {
      fver_t fver;
      TableFileReader reader(work_path_ + "/" + table_name_);

      Status status = reloadBefore();
      if (!status.ok()) {
        return status;
      }

      status = reader.open(fver);
      if (!status.ok()) {
        return status;
      }

      for (;;) {
        std::string data;

        status = reader.read(data);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
          break;
        }
        if (data.length() <= 0) {
          break;
        }
        CategoryDistribution distribution;

        if (!distribution.ParseFromString(data)) {
          reader.close();
          return Status::Corruption("Parse category distribution");
        }
        status = loadDistribution(distribution);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
        }
      }
      reader.close();

      return reloadCompleted();
    }

    Status DistTable::train()
    {
      Status status = trainBefore();
      if (!status.ok()) {
        return status;
      }

      status = trainPeriodicLog();
      if (!status.ok()) {
        return status;
      }
      std::string fullpath = work_path_ + "/" + table_name_;
      std::string tmp_fullpath =  fullpath + ".tmp";
      TableFileWriter writer(tmp_fullpath);

      status = writer.create(kDistTableVer);
      if (!status.ok()) {
        return status;
      }

      status = dumpTable(writer);
      if (!status.ok()) {
        return status;
      }
      writer.close();

      status = trainCompleted();
      if (!status.ok()) {
        return status;
      }

      if (::rename(tmp_fullpath.c_str(), fullpath.c_str())) {
        return Status::IOError(strerror(errno), ", oldfile=", tmp_fullpath, 
            ", newfile=", fullpath);
      }

      return Status::OK();
    }

    Status DistTable::rollover() 
    {
      return periodic_log_.rollover();
    }

    Status DistTable::eliminate() 
    {
      Status status = eliminateBefore();
      if (!status.ok()) {
        return status;
      }
 
      status = eliminatePeriodicLog();        
      if (!status.ok()) {
        return status;
      }

      return eliminateCompleted();
    }

    Status DistTable::recoveryAheadLog(const std::string& data)
    {
      const char* c_data = data.c_str();

      if (data[0] == kLogTypeClick) {
        CategoryClick click;

        if (!click.ParseFromArray(c_data + 1, data.length() - 1)) {
          LOG(WARNING) << "Parse category click failed";
          return Status::Corruption("Parse category click failed");
        }

        Status status = recoveryClick(click);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
          return status;
        }
      } else if (data[0] == kLogTypeItem) {
        CategoryItem item;

        if (!item.ParseFromArray(c_data + 1, data.length() - 1)) {
          LOG(WARNING) << "Parse category item failed";
          return Status::Corruption("Parse category item failed");
        }

        Status status = recoveryItem(item);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
          return status;
        }
      } else {
        LOG(WARNING) << "Invalid data";
        return Status::Corruption("Invalid data");
      }
      return Status::OK();
    }

    Status DistTable::trainPeriodicLog() 
    {
      PeriodicLog::Iterator iter = periodic_log_.getIterator();

      for (int j = 0; j < time_window_size_; j++, iter.next()) {
        if (!iter.isFileExist()) {
          LOG(WARNING) << "Periodic log lost: " << iter.filename();
          continue;
        }
        click_func_t clickf = bind(&DistTable::trainClick, this, _1);
        item_func_t itemf = bind(&DistTable::trainItem, this, _1);

        Status status = readPeriodicLog(iter.reader(), clickf, itemf);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
        }
      }

      return Status::OK();
    }

    Status DistTable::eliminatePeriodicLog() 
    {
      PeriodicLog::Iterator iter = periodic_log_.getIterator();

      for (int j = 0; j < time_window_size_; j++, iter.next()) {
        if (!iter.isFileExist()) {
          LOG(WARNING) << "Periodic log lost: " << iter.filename();
          continue;
        }

        if (!needEliminate(iter.last_counter(), iter.counter())) {
          continue;
        }
        click_func_t clickf = bind(&DistTable::eliminateClick, this, _1);
        item_func_t itemf = bind(&DistTable::eliminateItem, this, _1);

        Status status = readPeriodicLog(iter.reader(), clickf, itemf);
 
        std::string filename = iter.filename();
        if (::remove(filename.c_str())) {
          return Status::IOError(strerror(errno), ", file=", filename);
        }
      }

      return Status::OK();
    }

    Status DistTable::readPeriodicLog(PeriodicLog::Reader reader, click_func_t& clickf, item_func_t& itemf)
    {
      Status status = reader.open();
      if (!status.ok()) {
        return status;
      }
      for (;;) {
        std::string data;

        status = reader.read(data);
        if (!status.ok()) {
          break;
        }
        if (data.length() <= 0) {
          break;
        }
        const char* c_data = data.c_str();

        if (data[0] == kLogTypeClick) {
          CategoryClick click;

          if (!click.ParseFromArray(c_data + 1, data.length() - 1)) {
            LOG(WARNING) << "Parse category click failed";
            continue;
          }

          status = clickf(click);
          if (!status.ok()) {
            LOG(WARNING) << status.toString();
          }
        } else if (data[0] == kLogTypeItem) {
          CategoryItem item;

          if (!item.ParseFromArray(c_data + 1, data.length() - 1)) {
            LOG(WARNING) << "Parse category item failed";
            continue;
          }

          status = itemf(item);
          if (!status.ok()) {
            LOG(WARNING) << status.toString();
          }
        } else {
          LOG(WARNING) << "Invalid training data";
        }
      }
      reader.close();

      return Status::OK();
    }

    Status DistTable::addClick(const CategoryClick& click)
    {
      std::string serialized_data;

      serialized_data.append(1, kLogTypeClick);
      if (click.AppendToString(&serialized_data)) {
        return periodic_log_.write(serialized_data);
      }
      return Status::Corruption("Serialize click: user_id=", click.user_id(), 
          ", category_id=", click.category_id());
    }

    Status DistTable::addItem(const CategoryItem& item)
    {
      std::string serialized_data;

      serialized_data.append(1, kLogTypeItem);
      if (item.AppendToString(&serialized_data)) {
        return periodic_log_.write(serialized_data);
      }
      return Status::Corruption("Serialize click: item_id=", item.item_id(), 
          ", category_id=", item.category_id());
    }
  } // namespace recmd
} // namespace souyue
