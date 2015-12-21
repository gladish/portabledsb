#include "Bridge/ConfigManager.h"
#include "Bridge/IAdapter.h"
#include "Common/Log.h"

using namespace bridge;

namespace
{
  DSB_DECLARE_LOGNAME(ConfigManager);

  // TODO: make this configurable;
  std::string const kBridgeConfigFile = "BridgeConfig.xml";
  int const kMaxConnectionAttempts = 60;
  int const kReconnectDelay = 500;
  int const kDSBServicePort = 1000;
  uint32_t const kSessionLinkTimeout = 30; // seconds
}

ConfigManager::ConfigManager(DeviceSystemBridge& bridge, IAdapter& adapter)
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
  if (!m_busAttachment.get())
    return ER_OK;

  if (!m_serviceName.empty())
  {
    m_busAttachment->CancelAdvertiseName(m_serviceName.c_str(), ajn::TRANSPORT_ANY);
  }
  m_busAttachment->UnbindSessionPort(m_sessionPort);

  if (!m_serviceName.empty())
  {
    m_busAttachment->ReleaseName(m_serviceName.c_str());
  }

  m_busAttachment->Disconnect();

  // TODO: Destroy CSP interfaces
  // TODO: remove authentication handler
  // TODO: Shutdown About

  m_busAttachment->Stop();
  m_busAttachment->UnregisterBusListener(*this);
  m_busAttachment->Join();
  m_busAttachment.reset();

  return ER_OK;
}

QStatus
ConfigManager::ConnectToAllJoyn()
{
  QStatus st = BuildServiceName();
  if (st != ER_OK)
    return st;

  m_busAttachment.reset(new ajn::BusAttachment(m_adapter.GetExposedApplicationName().c_str(), true));
  m_busAttachment->RegisterBusListener(*this);
  m_busAttachment->Start();

  // TODO: AllJoyn about object

  // TODO: Initialize CSP bus objects

  st = m_busAttachment->Connect();
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to connect to AllJoyn bus: %d", st);
    return st;
  }

  st = m_busAttachment->RequestName(m_serviceName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to get name %s on AllJoyn bus: %d", m_serviceName.c_str(), st);
    return st;
  }

  ajn::SessionOpts sessionOpts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  st = m_busAttachment->BindSessionPort(m_sessionPort, sessionOpts, *this);
  if (!st)
  {
    DSBLOG_WARN("Failed to bind session port: %d", st);
    return st;
  }

  st = m_busAttachment->AdvertiseName(m_serviceName.c_str(), sessionOpts.transports);
  if (!st)
  {
    DSBLOG_WARN("Failed to advertise service name: %d", st);
    return st;
  }

  // TODO: Announce

  return ER_OK;
}

QStatus
ConfigManager::BuildServiceName()
{
  return ER_NOT_IMPLEMENTED;
}

bool
ConfigManager::AcceptSessionJoiner(ajn::SessionPort port, const char *joiner, const ajn::SessionOpts&)
{
  return port == m_sessionPort;
}

void
ConfigManager::SessionJoined(ajn::SessionPort port, ajn::SessionId id, const char *joiner)
{
  m_busAttachment->EnableConcurrentCallbacks();
  QStatus st = m_busAttachment->SetSessionListener(id, this);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to set session listener: %d", st);
    return;
  }

  uint32_t timeout = kSessionLinkTimeout;
  st = m_busAttachment->SetLinkTimeout(id, timeout);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to set session link timeout to %" PRIu32 ": %d", timeout, st);
    return;
  }
}

void
ConfigManager::SessionMemberRemoved(ajn::SessionId, const char *uniqueName)
{
  // TODO: Reset auth access
  // TODO: End CSP file transfer
}
