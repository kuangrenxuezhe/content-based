#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include <map>
#include <vector>
#include <string>

#include "gflags/gflags.h"
#include "utils/table_file.h"
#include "utils/util.h"
#include "utils/char_conv.h"
#include "proto/message.pb.h"
#include "proto/supplement.pb.h"

using namespace souyue::recmd;
using namespace module::protocol;

typedef std::vector<std::string>                vector_str_t;
typedef std::map<std::string, int>             map_str_int_t;
typedef std::map<int, int>                     map_int_int_t;
typedef std::map<int, double>               map_int_double_t;
typedef std::map<uint64_t, map_int_double_t> map_uint64_map_int_double_t;

static const fver_t kTableVer(1,0);

DEFINE_string(c, "category.tab", "Category");
DEFINE_string(nt, "contentbased.class.tag", "NewsTrends");
DEFINE_string(idx, "contentbased.idx", "UserInterestsIndex");
DEFINE_string(dat, "contentbased.dat", "UserInterestsData");
DEFINE_string(ntab, "news-trends.dat", "NewsTrendsTable");
DEFINE_string(utab, "user-interests.dat", "UserInterestsTable");

// 加载分类ID对照表
//   文件格式：categoryID|categoryName
void loadCategoryTable(map_str_int_t& table) {
  FILE* rfd = fopen(FLAGS_c.c_str(), "rb");
  if (NULL == rfd) {
    fprintf(stderr, "%s: %s\n", strerror(errno), FLAGS_c.c_str());
    exit(-1);
  }
  int total = 0;
  char line[300];

	while (fgets(line, 300, rfd)) {
		char* p = line+strlen(line);

		while (p>line && ('\r'==*(p-1) || '\n'==*(p-1))) {
			-- p;
    }

		*p = 0;
		if (0 == *line) {
			continue;
    }
    vector_str_t ss;

		splitString(line, '|', ss);
		if (ss.size() != 2) {
      fprintf(stderr, "Split: %s\n", toUTF8(line).c_str());
			continue;
    }

    if (!table.insert(std::make_pair(ss[1], atoi(ss[0].c_str()))).second) {
      fprintf(stderr, "InsertToMap: %s\n", toUTF8(line).c_str());
    }
    total++;
	}
	fclose(rfd);
  fprintf(stdout, "Load category: %d\n", total);
}

// 加载当前趋势并生成ID对照表
//   格式：categoryID|categoryName|currentTrend
// 注：由于提供的数据与系统中的ID不对照，这里需要创建一个对照表
void loadNewsTrends(const map_str_int_t& table, map_int_double_t& trends, map_int_int_t& idtab) {
  FILE* rfd = fopen(FLAGS_nt.c_str(), "rb");
  if (NULL == rfd) {
    fprintf(stderr, "%s: %s\n", strerror(errno), FLAGS_nt.c_str());
    exit(-1);
  }
  int total = 0;
  char line[300];

	while (fgets(line, 300, rfd)) {
		char* p = line+strlen(line);

		while (p>line && ('\r'==*(p-1) || '\n'==*(p-1))) {
			-- p;
    }

		*p = 0;
		if (0 == *line) {
			continue;
    }
    vector_str_t ss;

		splitString(line, '|', ss);
		if (ss.size() != 3) {
			continue;
    }
    int trainCategoryID = atoi(ss[0].c_str());

    map_str_int_t::const_iterator iter = table.find(ss[1]);
    if (iter == table.end()) {
      fprintf(stderr, "InvalidCategoryName: %s\n", toUTF8(line).c_str());
      continue;
    }
    double weight = atof(ss[2].c_str());

    if (!trends.insert(std::make_pair(iter->second, weight)).second) {
      fprintf(stderr, "InsertToNewsTrendsMap: %s\n", toUTF8(line).c_str());
      continue;
    }

    if (!idtab.insert(std::make_pair(trainCategoryID, iter->second)).second) {
      fprintf(stderr, "InsertToIDMap: %s\n", toUTF8(line).c_str());
    }
    total++;
	}
	fclose(rfd);
  fprintf(stdout, "Load news-trends: %d\n", total);
}

// 加载用户长期兴趣
void loadUserInterests(map_uint64_map_int_double_t& interests) {
  FILE* idxfd = fopen(FLAGS_idx.c_str(), "rb");
  if (NULL == idxfd) {
    fprintf(stderr, "%s: %s\n", strerror(errno), FLAGS_idx.c_str());
    exit(-1);
  }

  FILE* datfd = fopen(FLAGS_dat.c_str(), "rb");
  if (NULL == idxfd) {
    fprintf(stderr, "%s: %s\n", strerror(errno), FLAGS_dat.c_str());
    exit(-1);
  }
  int total = 0;
 
  uint64_t uid; int cnt;
	while (1 == fread(&uid, sizeof(uint64_t), 1, idxfd) && 1 == fread(&cnt, sizeof(int), 1, idxfd)) {
		if (0 == cnt) {
      continue;
    }
		interests[uid] = map_int_double_t(); 

		for (int i = 0; i < cnt; ++i) {
			int cid; double pr;

			if (1 != fread(&cid, sizeof(int), 1, datfd) || 1 != fread(&pr, sizeof(double), 1, datfd)) {
				fprintf(stderr, "Read failed: %s.\n", FLAGS_dat.c_str());
        exit(-1);
			}			
			interests[uid][cid] = pr;
		}
    total++;
	}
  fclose(idxfd); fclose(datfd);
  fprintf(stdout, "Load user interests: %d\n", total);
}

// Dump当前兴趣
void dumpNewsTrends(const map_int_double_t& trends) {
  CategoryDistribution dist;
  TableFileWriter tabfile(FLAGS_ntab);

  Status status = tabfile.create(kTableVer);
  if (!status.ok()) {
    fprintf(stderr, "%s\n", status.toString().c_str());
    exit(-1);
  }

  map_int_double_t::const_iterator iter = trends.begin();
  for (; iter != trends.end(); ++iter) {
      ItemTag* tag = dist.add_distribution();
      tag->set_tag_id(iter->first);
      tag->set_tag_power(iter->second);
  }
  std::string data = dist.SerializeAsString();
  tabfile.write(data);
  tabfile.close();
}

// Dump用户长期兴趣
void dumpUserInterests(const map_int_int_t& idtab, const map_uint64_map_int_double_t& interests) {
  TableFileWriter tabfile(FLAGS_utab);

  Status status = tabfile.create(kTableVer);
  if (!status.ok()) {
    fprintf(stderr, "%s\n", status.toString().c_str());
    exit(-1);
  }

  int total = 0;
  map_uint64_map_int_double_t::const_iterator iter = interests.begin();
  for (; iter != interests.end(); ++iter) {
    CategoryDistribution dist;

    dist.set_user_id(iter->first);

    map_int_double_t::const_iterator itv = iter->second.begin();
    for (; itv != iter->second.end(); ++itv) {
      ItemTag* tag = dist.add_distribution();
      tag->set_tag_id(idtab.find(itv->first)->second);
      tag->set_tag_power(itv->second);
    }
    std::string data = dist.SerializeAsString();
	  total++;
    tabfile.write(data);
  }
  tabfile.close();
}

int main(int argc, char** argv)
{
  map_str_int_t category_table;
  map_int_int_t category_id_table;

  map_int_double_t news_trends;
  map_uint64_map_int_double_t user_interests;

  gflags::ParseCommandLineFlags(&argc, &argv, false);

  loadCategoryTable(category_table);
  loadNewsTrends(category_table, news_trends, category_id_table);
  loadUserInterests(user_interests);

  dumpNewsTrends(news_trends);
  dumpUserInterests(category_id_table, user_interests);

  return 0;
}

