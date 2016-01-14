#pragma once

#include "Bridge/BridgeConfig.h"
#include "Bridge/AllJoynHeaders.h"

#include <memory>

namespace bridge
{
  class IAdapter;
  class DeviceSystemBridge;

  class ConfigManager : private ajn::BusListener, private ajn::SessionListener, private ajn::SessionPortListener
  {
  public:
    ConfigManager(DeviceSystemBridge&, IAdapter&);
    ~ConfigManager();

    QStatus Initialize();
    QStatus ConnectToAllJoyn();
    QStatus Shutdown();

    bool IsConfigurationAccessSecured();
    bool IsDeviceAccessSecured();
    bool GetObjectConfigItem();
    void ToFile();

    std::shared_ptr<BridgeConfig> GetBridgeConfig();

  private:
    QStatus ShutdownAllJoyn();
    QStatus InitializeCSPBusObjects();
    QStatus BuildServiceName();

    /* ajn::SessionPortListener */
    virtual bool AcceptSessionJoiner(ajn::SessionPort, const char *joiner, const ajn::SessionOpts&);
    virtual void SessionJoined(ajn::SessionPort, ajn::SessionId, const char *joiner);

    /* ajn::SessionListener */
    virtual void SessionMemberRemoved(ajn::SessionId, const char *uniqueName);

  private:
    DeviceSystemBridge& m_parent;
    IAdapter& m_adapter;
    std::shared_ptr<ajn::BusAttachment> m_busAttachment;
    std::string m_serviceName;

    // BridgeAuthHandler
    BridgeConfig m_bridgeConfig;
    ajn::SessionPort m_sessionPort;
  };
};

