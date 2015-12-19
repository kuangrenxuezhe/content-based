#include "core/content_based.h"
#include "db/db_type.h"
#include "utils/file.h"
#include "utils/duration.h"
#include "glog/logging.h"

namespace souyue {
  namespace recmd {
    static const fver_t kModelLockVer(1,0);
    static const std::string kModelLock = "/._cb_model_lock_";
    // 保存全局分类对照表
    std::map<int32_t, std::string> kCategory;
    bool kIsCategoryModified = false;
    pthread_mutex_t kCategoryLock = PTHREAD_MUTEX_INITIALIZER;

    class DurationLogger: public AutoDuration {
      public:
        template<typename... Args>
          DurationLogger(Duration::Unit unit, Args... args)
          : AutoDuration(unit, args...) {
          }

        virtual ~DurationLogger() {
          if (duration().count() < 100) {
            LOG(INFO) << info() << ", used: " << duration().count() << "ms";
          } else {
            LOG(WARNING) << info() << ", used: " << duration().count() << "ms";
          }
        }
    };

    Status loadCategory(const std::string& name) 
    {
      char buf[1024];

      FILE* rfd = fopen(name.c_str(), "rb");
      if (NULL == rfd) {
        Status status = Status::IOError(strerror(errno), ", file=", name);
        pthread_mutex_unlock(&kCategoryLock);
        return status;
      }
      while (fgets(buf, 1022, rfd)) {
        int len = strlen(buf);
        while (len > 0 && buf[len-1] > 0 && buf[len-1] <= 0x20)
          len--;
        if ('#' == buf[0] || len <= 0)
          continue;
        buf[len] = 0;

        char* p = strchr(buf, '|');
        if (NULL == p) {
          LOG(WARNING) << "Invalid category: " << buf;
          continue;
        }
        
        int32_t category_id = atoi(buf);
        if (category_id < 0)  {
          LOG(WARNING) << "Invalid category id: " << buf;
          continue;
        }
        // 若出现重复则最后一个
        std::map<int32_t, std::string>::iterator iter = kCategory.find(category_id);
        if (iter != kCategory.end()) {
          iter->second = std::string(p + 1);
        } else {
          kCategory.insert(std::make_pair(category_id, std::string(p + 1))); 
        }
      }
      fclose(rfd);

      return Status::OK();
    }

    Status flushCategory(const std::string& name) 
    {
      std::string temp_name = name + ".tmp";

      pthread_mutex_lock(&kCategoryLock);
      FILE* wfd = fopen(temp_name.c_str(), "wb");
      if (NULL == wfd) {
        Status status = Status::IOError(strerror(errno), ", file=", temp_name);
        pthread_mutex_unlock(&kCategoryLock);
        return status;
      }

      std::map<int32_t, std::string>::iterator iter = kCategory.begin();
      for (; iter != kCategory.end(); ++iter) {
        fprintf(wfd, "%d|%s\n", iter->first, iter->second.c_str());
      }
      fclose(wfd);
      kIsCategoryModified = false;
      pthread_mutex_unlock(&kCategoryLock);

      if (0 == rename(temp_name.c_str(), name.c_str())) {
        return Status::OK();
      }
      return Status::IOError(strerror(errno), ", oldfile=", temp_name, ", newfile=", name);
    }

    Status ContentBased::openModel(const ModelOptions& opts, ContentBased** model_ptr)
    {
      DurationLogger duration(Duration::kMilliSeconds, "OpenModel");
      ContentBased* c_model_ptr = new ContentBased(opts);

      Status status = c_model_ptr->lock();
      if (!status.ok()) {
        delete c_model_ptr; 
        return status;
      }
      *model_ptr = c_model_ptr;

      return (*model_ptr)->reload();
    }

    ContentBased::ContentBased(const ModelOptions& opts)
      : options_(opts), model_lock_(opts.work_path + kModelLock), 
      user_db_(NULL), item_db_(NULL), 
      marshaler_(NULL), news_trends_(NULL), user_interests_(NULL)
    {
      user_db_ = new UserDB(opts);
      item_db_ = new ItemDB(opts);
      marshaler_ = new Marshaler(opts);
      news_trends_ = new NewsTrends(opts);
      user_interests_ = new UserInterests(opts);
    }

    ContentBased::~ContentBased()
    {
      if (user_db_)
        delete user_db_;
      if (item_db_)
        delete item_db_;
      if (marshaler_)
        delete marshaler_;
      if (news_trends_)
        delete news_trends_;
      if (user_interests_)
        delete user_interests_;
    }

    // 可异步方式，线程安全
    Status ContentBased::flush()
    {
      DurationLogger duration(Duration::kMilliSeconds, "Flush");
      Status status = Status::OK();

      if (kIsCategoryModified) {
        status = flushCategory(options_.category_table_name);
        if (!status.ok()) {
          LOG(WARNING) << status.toString();
        }
      }
      
      status = user_db_->flushTable();
      if (status.ok()) {
        return status;
      }

      status = item_db_->flushTable();
      if (status.ok()) {
        return status;
      }

      return Status::OK();
    }

    Status ContentBased::train() 
    {
      DurationLogger duration(Duration::kMilliSeconds, "Train");
      // 新闻趋势在线学习，用户趋势离线学习
      return news_trends_->train();
    }

    Status ContentBased::reload()
    {
      DurationLogger duration(Duration::kMilliSeconds, "Reload");
      static bool kIsInitModel = true;

      Status status = Status::OK();
      if (kIsInitModel) {
        // 初始化需要加载用户表，Item表，分类表
        status = user_db_->loadTable();
        if (!status.ok()) {
          return status;
        }

        status = item_db_->loadTable();
        if (!status.ok()) {
          return status;
        }

        status = loadCategory(options_.category_table_name);
        if (!status.ok()) {
          return status;
        }

        status = news_trends_->init();
        if (!status.ok()) {
          return status;
        }

        status = user_interests_->init();
        if (!status.ok()) {
          return status;
        }
        kIsInitModel = false;
      }

      status = news_trends_->reload();
      if (!status.ok()) {
        return status;
      }

      return user_interests_->reload();
    }

    // 单进程锁定
    Status ContentBased::lock()
    {
      Status status = model_lock_.create();
      if (!status.ok()) {
        return status;
      }

      status = model_lock_.lock();
      if (!status.ok()) {
        return status;
      }

      return Status::OK();
    }

    // 查询当前已有的分类信息
    Status ContentBased::queryCategory(RepeatedKeyPair& category)
    {
      DurationLogger duration(Duration::kMilliSeconds, "QueryCategory");

      pthread_mutex_lock(&kCategoryLock);
      std::map<int32_t, std::string>::iterator iter = kCategory.begin();
      category.mutable_key_pair()->Reserve(kCategory.size());
      for (; iter != kCategory.end(); ++iter) {
        KeyPair* key_pair = category.add_key_pair();
        key_pair->set_key(iter->first);
        key_pair->set_name(iter->second);
      }
      pthread_mutex_unlock(&kCategoryLock);
      duration.appendInfo(": size=", category.key_pair_size());

      return Status::OK();
    }

    // 预测用户的当前兴趣
    Status ContentBased::predictUserInterests(const Category& category, AlgorithmCategory& dist)
    {
      DurationLogger duration(Duration::kMilliSeconds, "PredictUserInterests: user_id=", 
          category.user_id(), ", request_num=", category.request_num());

      vector_pair_t trends;
      Status status = user_interests_->queryUserInterests(category.user_id(), trends);
      if (!status.ok() || trends.size() <= 0) {
        status = user_interests_->queryCurrentUserInterests(category.user_id(), trends);
        if (!status.ok() || trends.size() <= 0) {
          status = news_trends_->queryCurrentTrends(trends);
        }
      }
      if (!status.ok() || trends.size() <= 0) {
        return status;
      }

      vector_pair_t interests;

      interests.reserve(category.request_num());
      status = marshaler_->marshal(trends, category.request_num(), interests);
      if (!status.ok()) {
        return status;
      }

      vector_pair_t::iterator iter = interests.begin();
      for (; iter != interests.end(); ++iter) {
        dist.add_category(iter->first);
      }
      duration.appendInfo(", interests size=", dist.category_size());

      return Status::OK();
    }

    // 用户点击更新
    Status ContentBased::updateAction(Action& action)
    {
      DurationLogger duration(Duration::kMilliSeconds, "UpdateAction: user_id=", 
          action.user_id(), ", item_id=", action.item_id());
      int32_t last_modified;
      Status status = Status::OK();
      
      bool isFind = user_db_->findUserReaded(action.user_id(), action.item_id(), last_modified);
      if (isFind && last_modified > 0) {
        // 不记录重复点击
        return Status::OK();
      }

      status = user_db_->updateAction(action);
      if (status.ok()) {
        return status;
      }
      RepeatedKeyPair belongs_to;
      
      status = item_db_->fetchItemBelongsTo(action.item_id(), belongs_to);
      if (!status.ok()) {
        return status;
      }
      CategoryClick click;

      click.set_user_id(action.user_id());
      click.set_publish_time(action.click_time());
      for (int i = 0; i < belongs_to.key_pair_size(); ++i) {
        type_id_t id;
        const KeyPair& key_pair = belongs_to.key_pair(i);

        id.type_id = key_pair.key();
        if (id.type_id_component.type != IDTYPE_CATEGORY)
          continue;

        click.set_category_id(id.type_id_component.id);

        status = news_trends_->addClick(click);
        if (!status.ok()) {
          return status;
        }

        status = user_interests_->addClick(click);
        if (!status.ok()) {
          return status;
        }
      }
      return Status::OK();
    }

    // 添加&更新新闻数据
    Status ContentBased::updateItem(const Item& item)
    {
      DurationLogger duration(Duration::kMilliSeconds, "UpdateItem: item_id=", item.item_id());

      Status status = item_db_->addItem(item);
      if (!status.ok()) {
        return status;
      }
      CategoryItem citem;

      citem.set_item_id(item.item_id());
      citem.set_publish_time(item.publish_time());
      for (int i = 0; i < item.category_size(); i++) {
        citem.set_category_id(item.category(i).tag_id());

        pthread_mutex_lock(&kCategoryLock);
        std::map<int32_t, std::string>::iterator iter;
        iter = kCategory.find(citem.category_id());
        if (iter != kCategory.end()) {
          if (iter->second != item.category(i).tag_name()) {
            iter->second = item.category(i).tag_name();
            kIsCategoryModified = true;
          }
        } else {
          kCategory.insert(std::make_pair(citem.category_id(), item.category(i).tag_name()));
          kIsCategoryModified = true;
        }
        pthread_mutex_unlock(&kCategoryLock);

        status = user_interests_->addItem(citem);
        if (!status.ok()) {
          return status;
        }
      }
      return Status::OK();
    }

    // 查询新闻趋势
    Status ContentBased::queryNewsTrends(CategoryDistribution& dist)
    {
      DurationLogger duration(Duration::kMilliSeconds, "QueryNewsTrends");

      vector_pair_t trends;
      Status status = news_trends_->queryCurrentTrends(trends);
      if (!status.ok()) {
        return status;
      }
      vector_pair_t::iterator iter = trends.begin();
      for (; iter != trends.end(); ++iter) {
        ItemTag* tag = dist.add_distribution();
        tag->set_tag_id(iter->first);
        tag->set_tag_power(iter->second);
      }
      duration.appendInfo(": trends size=", dist.distribution_size());

      return Status::OK();
    }

    // 查询用户历史兴趣
    Status ContentBased::queryUserInterests(const Category& query, CategoryDistribution& dist)
    {
      DurationLogger duration(Duration::kMilliSeconds, "QueryUserInterests: user_id=", 
          query.user_id(), ", request_num=", query.request_num());

      vector_pair_t trends;
      Status status = user_interests_->queryUserInterests(query.user_id(), trends);
      if (!status.ok() || trends.size() <= 0) {
        status = user_interests_->queryCurrentUserInterests(query.user_id(), trends);
      }
      if (!status.ok()) {
        return status;
      }

      vector_pair_t::iterator iter = trends.begin();
      for (; iter != trends.end(); ++iter) {
        ItemTag* tag = dist.add_distribution();
        tag->set_tag_id(iter->first);
        tag->set_tag_power(iter->second);
      }
      duration.appendInfo(", trends size=", dist.distribution_size());

      return Status::OK();
    }
  } // namespace recmd
} // namespace souyue