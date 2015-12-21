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

CXXFLAGS=-D QCC_OS_GROUP_POSIX -Wall -Wextra -Wno-missing-field-initializers -Wno-deprecated-declarations -g -std=c++0x -I. -I$(ALLJOYN_INSTALL_DIR)/inc -I$(LIBXML_INC)
CXXFLAGS+=-Wno-ignored-qualifiers
LDFLAGS=-L $(ALLJOYN_INSTALL_DIR)/lib -lalljoyn -lcrypto -lxml2 
DEV_PROVIDER_OBJS=$(patsubst %.cpp, %.o, $(SRCS))
OBJS=$(DEV_PROVIDER_OBJS)

all: moc-adapter

clean:
	$(RM) moc-adapter *.o DeviceProviders/*.o Bridge/*.o Common/*.o Adapters/MockAdapter/*.o \
    ZigBeeAdapter/*.o



moc-adapter: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

