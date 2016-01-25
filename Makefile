SRCS=DeviceProviders/AllJoynProvider.cpp \
     DeviceProviders/AllJoynService.cpp \
     Bridge/AllJoynAbout.cpp \
     Bridge/AllJoynHelper.cpp \
     Bridge/Bridge.cpp \
     Bridge/BridgeBusObject.cpp \
     Bridge/BridgeConfig.cpp \
     Bridge/ConfigManager.cpp \
     Common/AdapterLog.cpp \
     Common/Variant.cpp \
     Common/Guid.cpp \
     Adapters/MockAdapter/MockAdapter.cpp \
     Adapters/MockAdapter/MockDevices.cpp \
     Adapters/MockAdapter/ZigBeeClusterDatabase.cpp \
     main.cpp

TEST_SRCS = Tests/VariantTest.cpp
TEST_OBJS = $(patsubst %.cpp, %.o, $(TEST_SRCS))
TESTS = $(patsubst %.cpp, %, $(TEST_SRCS))

LIBXML_INC?=/usr/include/libxml2

ALLJOYN_INSTALL_DIR?=/Users/jgladi200/Work/alljoyn/alljoyn-15.09.00a-src/build/darwin/x86/debug/dist/cpp

CXXFLAGS          = -Wall -Wextra -std=c++11 -I. -I$(ALLJOYN_INSTALL_DIR)/inc -I$(LIBXML_INC) -Wno-unused-parameter
LDFLAGS           = -L $(ALLJOYN_INSTALL_DIR)/lib -lalljoyn -lcrypto -lxml2
DEV_PROVIDER_OBJS = $(patsubst %.cpp, %.o, $(SRCS))
OBJS              = $(DEV_PROVIDER_OBJS)
DEPS              = $(OBJS:%.o=%.d) $(TEST_OBJS:%.o=%.d)

GTEST_DIR?=/usr/src/gtest
ifneq ("$(wildcard $(GTEST_DIR)/libgtest.a)","")
	GTEST_FLAGS = $(GTEST_DIR)/libgtest.a
	CXXFLAGS += -I$(GTEST_DIR)/include
else
	GTEST_FLAGS = -lgtest
endif

UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -pthread -luuid
    CXXFLAGS += -DQCC_OS_GROUP_POSIX
endif

ifeq ($(DEBUG), 1)
  CXXFLAGS += -g -O0 -fno-inline
else
  CXXFLAGS += -O2
endif

ifeq ($V, 1)
  CXX_PRETTY = $(CXX)
  LD_PRETTY = $(CXX)
else
  CXX_PRETTY = @echo " [CXX] $<" ; $(CXX)
  LD_PRETTY = @echo "[LINK] $@" ; $(CXX)
endif

all: moc-adapter

tests: $(TESTS)

check: tests
	@for test in $(TESTS) ; do \
		$$test ; \
	done

clean:
	$(RM) moc-adapter $(TESTS) $(OBJS) $(DEPS) $(TEST_OBJS)

moc-adapter: $(OBJS)
	$(LD_PRETTY) -o $@ $^ $(LDFLAGS)

Tests/VariantTest: Tests/VariantTest.o Common/Variant.o
	$(LD_PRETTY) -o $@ $^ $(LDFLAGS) $(GTEST_FLAGS)

%.o: %.cpp
	$(CXX_PRETTY) $(CXXFLAGS) -MMD -c -o $@ $<

-include $(DEPS)
