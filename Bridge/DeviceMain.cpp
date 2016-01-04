#include "DeviceMain.h"

#include "AllJoynHelper.h"
#include "Common/Log.h"

namespace
{
  DSB_DECLARE_LOGNAME(DeviceMain);
}

static std::string BuildBusObjectPath(const std::string& name)
{
  std::string encodedName;
  bridge::AllJoynHelper::EncodeBusObjectName(name, encodedName);
  return "/" + encodedName;
}

bridge::DeviceMain::DeviceMain(BridgeDevice& parent)
  : ajn::BusObject(BuildBusObjectPath(m_parent.GetAdapterDevice()->GetName()).c_str(), false)
  , m_parent(parent)
  , m_indexForSignal(1)
  , m_indexForMethod(1)
  , m_registeredOnAllJoyn(false)
{
}

bridge::DeviceMain::~DeviceMain()
{
  Shutdown();
}

void
bridge::DeviceMain::Shutdown()
{
}

QStatus
bridge::DeviceMain::Initialize()
{
  QStatus st = ER_OK;

  st = CreateMethodsAndSignals();
  if (st != ER_OK)
    return st;

  st = AddInterface(*m_interfaceDescription);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to add interface to bus object: %s", QCC_StatusText(st));
    return st;
  }

  for (std::map<std::string, DeviceMethod*>::iterator i = m_deviceMethods.begin(); i != m_deviceMethods.end(); ++i)
  {
    const char* methodName = i->first.c_str();
    const ajn::InterfaceDescription::Member* member = m_interfaceDescription->GetMember(methodName);
    if (!member) {
      DSBLOG_WARN("Failed to find InterfaceDescriptionMember for %s", methodName);
      return ER_INVALID_DATA;
    }

    st = AddMethodHandler(member, static_cast<ajn::MessageReceiver::MethodHandler>(&DeviceMain::AJMethod));
    if (st != ER_OK)
    {
      DSBLOG_WARN("Failed to add method handler: %s", QCC_StatusText(st));
      return st;
    }
  }

  st = m_parent.GetBusAttachment().RegisterBusObject(*this);
  if (st != ER_OK)
  {
    DSBLOG_WARN("Failed to register bus object: %s", QCC_StatusText(st));
    return st;
  }

  m_registeredOnAllJoyn = true;
  return st;
}

bool
bridge::DeviceMain::IsMethodNameUnique(std::string const& name)
{
  return m_deviceMethods.find(name) != m_deviceMethods.end();
}

bool
bridge::DeviceMain::IsSignalNameUnique(std::string const&)
{
  // TODO:
  return false;
}

QStatus
bridge::DeviceMain::CreateMethodsAndSignals()
{
  // TODO
  DSBLOG_NOT_IMPLEMENTED();
  return ER_NOT_IMPLEMENTED;
}

void
bridge::DeviceMain::AJMethod(const ajn::InterfaceDescription::Member*, ajn::Message&)
{
}
