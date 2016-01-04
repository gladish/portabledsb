SRCS=DeviceProviders/AllJoynProvider.cpp \
     DeviceProviders/AllJoynService.cpp \
     Bridge/AllJoynAbout.cpp \
     Bridge/AllJoynHelper.cpp \
     Bridge/Bridge.cpp \
     Bridge/BridgeConfig.cpp \
     Bridge/BridgeDevice.cpp \
     Bridge/ConfigManager.cpp \
     Bridge/DeviceMain.cpp \
     Bridge/DeviceMethod.cpp \
     Bridge/DeviceProperty.cpp \
     Bridge/DeviceSignal.cpp \
     Common/Log.cpp \
     Common/Variant.cpp \
     Adapters/MockAdapter/MockAdapter.cpp \
     Adapters/MockAdapter/MockDevices.cpp \
     Adapters/MockAdapter/MockAdapterDevice.cpp \
     Adapters/ZigBeeAdapter/ZigBeeAdapter.cpp \
     main.cpp

LIBXML_INC?=/usr/include/libxml2

ALLJOYN_INSTALL_DIR?=/Users/jgladi200/Work/alljoyn/alljoyn-15.09.00a-src/build/darwin/x86/debug/dist/cpp

CXXFLAGS=-D QCC_OS_GROUP_POSIX -Wall -Wextra -Wno-missing-field-initializers -Wno-deprecated-declarations -Wno-ignored-qualifiers -std=c++0x -I. -I$(ALLJOYN_INSTALL_DIR)/inc -I$(LIBXML_INC)
LDFLAGS=-L $(ALLJOYN_INSTALL_DIR)/lib -lalljoyn -lcrypto -lxml2
DEV_PROVIDER_OBJS=$(patsubst %.cpp, %.o, $(SRCS))
OBJS=$(DEV_PROVIDER_OBJS)
DEPS = $(OBJS:%.o=%.d)

UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -pthread -luuid
endif

ifeq ($(DEBUG), 1)
CXXFLAGS += -g
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

clean:
	$(RM) moc-adapter $(OBJS) $(DEPS)

moc-adapter: $(OBJS)
	$(LD_PRETTY) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX_PRETTY) $(CXXFLAGS) -MMD -c -o $@ $<

-include $(DEPS)
