#pragma once

#include "Bridge/BridgeConfig.h"
#include "Bridge/AllJoynHeaders.h"
#include "Common/Adapter.h"

#include <memory>

namespace bridge
{
  class Bridge;

  class ConfigManager : private ajn::BusListener, private ajn::SessionListener, private ajn::SessionPortListener
  {
  public:
    ConfigManager(Bridge&, std::shared_ptr<adapter::Adapter> const& adapter);
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
    virtual void BusDisconnected();

  private:
    Bridge& m_parent;
    std::shared_ptr<adapter::Adapter>   m_adapter;
    std::unique_ptr<ajn::BusAttachment> m_bus;
    std::string m_serviceName;

    // BridgeAuthHandler
    BridgeConfig m_bridgeConfig;
    ajn::SessionPort m_sessionPort;
  };
};

