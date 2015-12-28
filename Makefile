BUILD=.build
TARGET=content-based
UNITTESTS=unittest

SRCPATH=./src/
OBJPATH=$(BUILD)/objs/

ifneq ($(strip $(debug)),)
	DEBUG=1
endif
DEBUG?=0

ifneq ($(strip $(prefix)),)
	PREFIX=$(prefix)
endif
PREFIX?=/usr

GCC_VERSION=$(shell g++ -dumpversion)

INCLUDES=-I./src -I./deps/include
LDFLAGS=-L./deps/lib -L$(BUILD)/lib

ifneq ($(strip $(DEBUG)), 1)
	CFLAGS+= -g -w -O2 -DNDEBUG
else
	CFLAGS+=-g -w -O0 -DDEBUG -DTRACE
endif

ifeq ($(GCC_VERSION), 4.8)
	CFLAGS+= -std=c++11 -DCPP11
else
	CFLAGS+= -std=c++0x
endif

.SUFFIXES:
.PHONY: build install check rebuild uninstall clean help

LIBS=-lrdkafka++ -lrdkafka -ljson -ldb -lutils -luuid -lglog \
		 -lgflags -lprotobuf -lpthread -lcrypto -lconfig++ \
		 -lgrpc -lgpr -lgrpc_unsecure -lgrpc++_unsecure -lz

sources=main.cpp \
				core/marshaler.cpp \
				core/periodic_log.cpp \
				core/model_options.cpp \
				core/dist_table.cpp \
				core/news_trends.cpp \
				core/user_interests.cpp \
				core/content_based.cpp \
				proto/supplement.pb.cpp \
				proto/service.pb.cpp \
				proto/service.grpc.pb.cpp \
				service/service_glue.cpp \
				service/service_grpc.cpp
				
unittests=unittest.cpp \
					core/marshaler.cpp \
					core/model_options.cpp \
					core/marshaler_test.cpp
					#core/marshaler.cpp \
					#core/periodic_log.cpp \
					#core/model_options.cpp \
					#core/dist_table.cpp \
					#core/news_trends.cpp \
					#core/user_interests.cpp \
					#core/content_based.cpp \
					#proto/supplement.pb.cpp \
					#core/periodic_log_test.cpp  \
					#core/dist_table_test.cpp \
					#core/news_trends_test.cpp \
					#core/user_interests_test.cpp \
					#core/content_based_test.cpp
					
OBJECTS=$(addprefix $(OBJPATH), $(sources:.cpp=.o))
OBJTESTS=$(addprefix $(OBJPATH), $(unittests:.cpp=.o))

build: mkdir $(TARGET) $(UNITTESTS)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD)/bin/
	$(CXX) -o $(BUILD)/bin/$@ $(CFLAGS) $(INCLUDES) $(LDFLAGS) $^ $(LIBS)

$(UNITTESTS): $(OBJTESTS)
	@mkdir -p $(BUILD)/bin/
	$(CXX) -o $(BUILD)/bin/$@ $(CFLAGS) $(INCLUDES) $(LDFLAGS) $^ $(LIBS)

sinclude $(addprefix $(OBJPATH), $(sources:.cpp=.d))
sinclude $(addprefix $(OBJPATH), $(unittests:.cpp=.d))
$(OBJPATH)%.o: $(SRCPATH)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -c -o $@
	@$(CXX) -MM $(CFLAGS) $(INCLUDES) $^ > $(OBJPATH)/$*.d
	@mv -f $(OBJPATH)/$*.d $(OBJPATH)/$*.d.tmp
	@sed -e 's|.*:|$(OBJPATH)/$*.o:|' < $(OBJPATH)/$*.d.tmp > $(OBJPATH)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJPATH)/$*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJPATH)/$*.d
	@rm -f $(OBJPATH)/$*.d.tmp

check: build 
	@$(BUILD)/bin/unittest

install: build
	@mkdir -p $(PREFIX)/bin
	@cp $(BUILD)/lib/$(TARGET) $(PREFIX)/lib/
	
rebuild: clean build

uninstall:
	@rm -rf $(PREFIX)/include/db
	@rm -rf $(PREFIX)/include/proto
	@rm -f $(PREFIX)/lib/$(TARGET)

clean:
	@rm -rf $(BUILD)

mkdir:
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/bin
	@mkdir -p $(BUILD)/objs

proto:
	protoc -I./docs -I../../deps/src/db/docs --cpp_out=./src/proto ./docs/supplement.proto ./docs/service.proto
	@sed "s/message.pb.h/proto\/message.pb.h/" ./src/proto/supplement.pb.h  > ./src/proto/supplement.pb.h.tmp
	@mv ./src/proto/supplement.pb.h.tmp ./src/proto/supplement.pb.h
	@mv ./src/proto/supplement.pb.cc ./src/proto/supplement.pb.cpp
	@sed "s/message.pb.h/proto\/message.pb.h/" ./src/proto/service.pb.h  > ./src/proto/service.pb.h.tmp
	@sed "s/supplement.pb.h/proto\/supplement.pb.h/" ./src/proto/service.pb.h.tmp  > ./src/proto/service.pb.h
	@rm ./src/proto/service.pb.h.tmp
	@mv ./src/proto/service.pb.cc ./src/proto/service.pb.cpp
	protoc -I./docs -I../../deps/src/db/docs --grpc_out=./src/proto --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin ./docs/service.proto
	@mv ./src/proto/service.grpc.pb.cc ./src/proto/service.grpc.pb.cpp

help:
	@echo "Usage: make [options] [target]"
	@echo "Options:"
	@echo "  debug[=FLAG]   : flag: 0 ndebug, 1 debug"
	@echo "  prefix[=PATH]  : install path, default: /usr/include"
	@echo "Target:"
	@echo "  build          : Build target"
	@echo "  install        : Install target to prefix path"
	@echo "  check          : Run unittest"
	@echo "  rebuild        : Rebuild target"
	@echo "  uninstall      : Uninstall from prefix path"
	@echo "  clean          : Clean target and objects"
	@echo "  help           : Print help"
