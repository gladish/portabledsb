SRCS=VariantTest.cpp
ALLJOYN_INSTALL_DIR?=/
GTEST_DIR?=/usr/src/gtest
LIBXML_INC?=/usr/include/libxml2

CXX_WARNINGS=-Wall -Wextra -Wno-missing-field-initializers -Wno-deprecated-declarations -Wno-ignored-qualifiers
CXX_DEFINES=-D QCC_OS_GROUP_POSIX
CXX_SEARCH=-I.. -I$(ALLJOYN_INSTALL_DIR)/inc -I$(LIBXML_INC) -I$(GTEST_DIR)/include
CXXFLAGS=-isystem -std=c++0x -g $(CXX_WARNINGS) $(CXX_DEFINES) $(CXX_SEARCH)

LDFLAGS=-L$(ALLJOYN_INSTALL_DIR)/lib -lalljoyn -lcrypto -lxml2 -L$(GTEST_DIR)/libgtest.a

TEST_OBJS=$(patsubst %.cpp, %.o, $(SRCS))
OBJS=$(TEST_OBJS)
DEPS = $(OBJS:%.o=%.d)

UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -pthread -luuid
endif

ifeq ($V, 1)
CXX_PRETTY = $(CXX)
LD_PRETTY = $(CXX)
else
CXX_PRETTY = @echo " [CXX] $<" ; $(CXX)
LD_PRETTY = @echo "[LINK] $@" ; $(CXX)
endif

ALL_TESTS=VariantTest

all: $(ALL_TESTS)
clean:
	$(RM) $(ALL_TESTS) $(OBJS) $(DEPS)

VariantTest: VariantTest.cpp ../Variant.cpp
	$(CXX) -isystem $(CXXFLAGS) $(LDFLAGS) $^ $(GTEST_DIR)/libgtest.a -o $@

%.o: %.cpp
	$(CXX_PRETTY) $(CXXFLAGS) -MMD -c -o $@ $<

%.o: $(GTEST_DIR)/src/%.cc
	$(CXX_PRETTY) $(CXXFLAGS) -MMD -c -o $@ $<

#-include $(DEPS)
