#include "Bridge/ConfigManager.h"
#include "Bridge/AllJoynHelper.h"
#include "Common/AdapterLog.h"

using namespace bridge;

namespace
{
  DSB_DECLARE_LOGNAME(ConfigManager);

  // TODO: make this configurable;
  std::string const kBridgeConfigFile = "BridgeConfig.xml";
  //int const kMaxConnectionAttempts = 60;
  //int const kReconnectDelay = 500;
  int const kDSBServicePort = 1000;
  uint32_t const kSessionLinkTimeout = 30; // seconds
}

ConfigManager::ConfigManager(Bridge& bridge, std::shared_ptr<adapter::Adapter> const& adapter)
  : m_parent(bridge)
  , m_adapter(adapter)
  , m_sessionPort(kDSBServicePort)
{
}

ConfigManager::~ConfigManager()
{
  Shutdown();
}

QStatus
ConfigManager::Initialize()
{
  return m_bridgeConfig.FromFile(kBridgeConfigFile);
}

QStatus
ConfigManager::Shutdown()
{
  QStatus st = ShutdownAllJoyn();
  return st;
}

QStatus
ConfigManager::ShutdownAllJoyn()
{
  if (!m_bus)
    return ER_OK;

  if (!m_serviceName.empty())
    m_bus->CancelAdvertiseName(m_serviceName.c_str(), ajn::TRANSPORT_ANY);
  m_bus->UnbindSessionPort(m_sessionPort);

  if (!m_serviceName.empty())
    m_bus->ReleaseName(m_serviceName.c_str());

  m_bus->Disconnect();

  // TODO: Destroy CSP interfaces
  // TODO: remove authentication handler
  // TODO: Shutdown About

  m_bus->Stop();
  m_bus->UnregisterBusListener(*this);
  m_bus->Join();
  m_bus.reset();

  return ER_OK;
}

QStatus
ConfigManager::ConnectToAllJoyn()
{
  QStatus st = BuildServiceName();
  if (st != ER_OK)
    return st;

  m_bus.reset(new ajn::BusAttachment(m_adapter->GetApplicationName().c_str(), true));
  m_bus->RegisterBusListener(*this);
  m_bus->Start();

  // TODO: AllJoyn about object

  // TODO: Initialize CSP bus objects

  bool failedToConnect = false;
  do
  {
    st = m_bus->Connect();
    if (st != ER_OK)
    {
      if (!failedToConnect)
        DSBLOG_WARN("Failed to connect to AllJoyn bus: %s", QCC_StatusText(st));
      failedToConnect = true;
      sleep(1);
    }
    else
    {
      failedToConnect = false;
    }
  }
  while (st != ER_OK);

  if (st == ER_OK)
    DSBLOG_INFO("connected to: %s", m_bus->GetConnectSpec().c_str());

  st = m_bus->RequestName(m_serviceName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to get name %s on AllJoyn bus: %s", m_serviceName.c_str(), QCC_StatusText(st));
    return st;
  }

  ajn::SessionOpts sessionOpts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  st = m_bus->BindSessionPort(m_sessionPort, sessionOpts, *this);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to bind session port: %s", QCC_StatusText(st));
    return st;
  }

  st = m_bus->AdvertiseName(m_serviceName.c_str(), sessionOpts.transports);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to advertise service name: %s", QCC_StatusText(st));
    return st;
  }

  // TODO: Announce

  return ER_OK;
}

QStatus
ConfigManager::BuildServiceName()
{
  m_serviceName.clear();

  std::string tmp = AllJoynHelper::EncodeStringForRootServiceName(m_adapter->GetAdapterPrefix());
  if (tmp.empty()) {
    return ER_BUS_BAD_BUS_NAME;
  }

  m_serviceName = tmp + ".Bridge";

  adapter::ItemInformation info = m_adapter->GetInfo();

  tmp = AllJoynHelper::EncodeStringForServiceName(info.GetName());
  if (tmp.empty()) {
    m_serviceName.empty();
    return ER_BUS_BAD_BUS_NAME;
  }

  m_serviceName += ".";
  m_serviceName += tmp;
  return ER_OK;
}

bool
ConfigManager::AcceptSessionJoiner(ajn::SessionPort port, const char*, const ajn::SessionOpts&)
{
  return port == m_sessionPort;
}

void
ConfigManager::SessionJoined(ajn::SessionPort, ajn::SessionId id, const char*)
{
  m_bus->EnableConcurrentCallbacks();
  QStatus st = m_bus->SetSessionListener(id, this);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to set session listener: %s", QCC_StatusText(st));
    return;
  }

  uint32_t timeout = kSessionLinkTimeout;
  st = m_bus->SetLinkTimeout(id, timeout);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to set session link timeout to %" PRIu32 ": %s", timeout, QCC_StatusText(st));
    return;
  }
}

void
ConfigManager::BusDisconnected()
{
  DSBLOG_WARN("someone please reconnect me!");
}

void
ConfigManager::SessionMemberRemoved(ajn::SessionId, const char*)
{
  // TODO: Reset auth access
  // TODO: End CSP file transfer
}
