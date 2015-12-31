#ifndef SOUYUE_RECMD_MODELS_CONTENT_BASED_MODEL_OPTIONS_H
#define SOUYUE_RECMD_MODELS_CONTENT_BASED_MODEL_OPTIONS_H

#include <stdint.h>
#include <string>
#include "utils/status.h"
#include "db/options.h"

namespace souyue {
  namespace recmd {
    // Timer，格式：NN/Gap
    //  NN: 有效Gap范围内的数值
    //  Gap：mon 月(1-31), week 周(0-6)，day 天(0-23), hour: 时(0-59)
    // 样例: 23/day, 表示每天的23点进行flush操作
    // 默认：23/day
    class ModelOptions: public Options {
      public:
        // GRPC服务端口, 默认: 6100
        int rpc_port;
        // 服务监控端口, 默认: 16100
        int monitor_port;

         // 默认：0.4
        float marshaler_r1;
        // 默认：0.6
        float marshaler_r2;
        //  默认：3
        int marshaler_topt;
        //  默认：4
        int marshaler_p;
        //  默认：2
        int marshaler_n;
        
        // db flush定时器, 默认：23/day
        std::string flush_timer;
        // 新闻趋势和用户当前兴趣训练定时器, 默认：50/hour
        std::string train_timer;
        // 滚存日志文件定时器, 默认：30/hour
        std::string rollover_timer;
        // 重新加载用户长期兴趣定时器, 默认：23/day
        std::string reload_timer;

        // 全局分类表名称, 默认：category.dat
        std::string category_table_name;

        // 当前新闻趋势表名, 默认：news-trends.dat
        std::string news_trends_table_name;
        // 当前新闻趋势log文件前缀, 默认：news-trends
        std::string news_trends_log_prefix;
        // 获取当前新闻趋势的时间窗口, 默认：48，单位小时
        int32_t news_trends_time_window;

        // 先验概率类型：0 点击分布，1 文档分布，默认：0
        int user_interests_prior_prob_type;
        // 用户模拟点击, 默认：10
        float user_interests_visual_clicks;
        // 当前新闻趋势表名, 默认：news-trends.dat
        std::string user_interests_table_name;
        // 当前新闻趋势log文件前缀, 默认：news-trends
        std::string user_interests_log_prefix;
        // 当前用户兴趣最大时间窗口, 默认: 24, 单位小时
        int32_t user_interests_time_window;
        // 融合用户兴趣，用户当前兴趣, 默认: 0.7
        float user_interests_mix_alpha;

      public:
        ModelOptions();
        // 从配置文件加载Option
        static Status fromConf(const std::string& conf, ModelOptions& opts);
    };
  } // namespace recmd
} // namespace souyue
#endif // #define SOUYUE_RECMD_MODELS_CONTENT_BASED_MODEL_OPTIONS_H

