//
// Copyright (c) 2016, Comcast Cable Communications Management, LLC
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or
// without fee is hereby granted, provided that the above copyright notice and this
// permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
// SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
// ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.
//
#include "bridge/BridgeBusObject.h"
#include "bridge/BridgeError.h"
#include "bridge/AllJoynHelper.h"
#include "common/AdapterLog.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

namespace
{
  DSB_DECLARE_LOGNAME(BusObject);

  // TODO: new class (InterfaceCache and/or InterfaceBuilder
  typedef std::map< std::string, std::shared_ptr<adapter::Interface> > InterfaceCache;
  InterfaceCache s_interfaceCache;

  uint8_t GetAccess(adapter::Property const& p)
  {
    uint8_t ret = 0;
    switch (p.GetAccess())
    {
      case adapter::AccessType::Read:
        ret = ajn::PROP_ACCESS_READ;
        break;
      case adapter::AccessType::Write:
        ret = ajn::PROP_ACCESS_WRITE;
        break;
      case adapter::AccessType::ReadWrite:
        ret = ajn::PROP_ACCESS_RW;
        break;
    }
    return ret;
  }

  // TODO: move this to some type of reconnecting BusAttachment
  // PersistentBusAttachment
  uint64_t                          reconnectRefcount = 0;
  bool                              reconnectShutdown;
  bool                              reconnectErrorState;
  std::unique_ptr<std::thread>      reconnectThread;
  std::mutex                        reconnectMutex;
  std::condition_variable           reconnectCond;
  std::vector<ajn::BusAttachment *> reconnectList;

  void reconnectFunction()
  {
    static const int millis = 1000;

    while (true)
    {
      auto delay = std::chrono::system_clock::now() + std::chrono::milliseconds(millis);

      std::unique_lock<std::mutex> lock(reconnectMutex);
      reconnectCond.wait_until(lock, delay, [] {
        return
          !reconnectList.empty() &&
          !reconnectErrorState &&
          !reconnectShutdown;
      });

      if (reconnectShutdown)
        return;

      reconnectErrorState = false;
      for (auto& busAttachment : reconnectList)
      {
        if (busAttachment == nullptr)
          continue;

        QStatus st = busAttachment->Connect();
        if (st == ER_OK || st == ER_BUS_ALREADY_CONNECTED)
        {
          DSBLOG_INFO("reconnected BusAttachment: %s/%s", busAttachment->GetGlobalGUIDString().c_str(),
            busAttachment->GetConnectSpec().c_str());
          busAttachment = nullptr;
        }
        else
        {
          DSBLOG_DEBUG("error trying to reconnect BusAttachment:%s", QCC_StatusText(st));
          reconnectErrorState = true;
        }
      }

      if (!reconnectList.empty())
      {
        auto end = std::remove_if(reconnectList.begin(), reconnectList.end(),
            [](ajn::BusAttachment* p) { return p == nullptr; });

        reconnectList.erase(end, reconnectList.end());
      }
    }
  }
}

bridge::BusObject::BusObject(std::string const& appname, std::string const& path, std::shared_ptr<ajn::BusAttachment> const& bus)
  : ajn::BusObject(path.c_str())
  , m_bus(bus)
{
  DSBLOG_INFO("new child: %s", path.c_str());
}

bridge::BusObject::BusObject(std::string const& appname, std::string const& path)
  : ajn::BusObject(path.c_str())
  , m_bus(new ajn::BusAttachment(appname.c_str(), true))
  , m_appName(appname)
{
  std::unique_lock<std::mutex> lock(reconnectMutex);
  reconnectRefcount++;
  if (!reconnectThread)
  {
    reconnectShutdown = false;
    reconnectThread.reset(new std::thread(reconnectFunction));
  }
}

bridge::BusObject::~BusObject()
{
  std::unique_lock<std::mutex> lock(reconnectMutex);
  reconnectRefcount--;
  if (reconnectRefcount == 0)
  {
    if (reconnectThread)
    {
      reconnectShutdown = true;
      lock.unlock();
      reconnectCond.notify_one();

      DSBLOG_INFO("waiting for reconnect thread to shutdown");
      reconnectThread->join();
    }
  }

  if (m_busListener)
  {
    m_bus->UnregisterBusListener(*m_busListener);
    m_busListener.reset();
  }

  if (m_bus->IsStarted())
    m_bus->Stop();

  QStatus st = ER_OK;

  if (m_aboutObject)
    m_aboutObject->Unannounce();

  m_aboutObject.reset();
  m_aboutData.reset();

  if (m_bus->IsConnected())
  {
    st = m_bus->Disconnect();
    if (st != ER_OK)
      DSBLOG_WARN("failed to disconnect BusAttachment: %s", QCC_StatusText(st));
  }

  m_bus->Join();
}

std::shared_ptr<bridge::BusObject>
bridge::BusObject::BuildFromAdapterDevice(std::string const& appname, std::string const& path,
  std::shared_ptr<adapter::Adapter> const& adapter, adapter::Device const& dev)
{
  QStatus st = ER_OK;

  std::shared_ptr<BusObject> obj(new BusObject(appname, path));
  obj->m_adapter = adapter;
  obj->m_adapterDevice = dev;

  DSB_ASSERT(!obj->m_sessionPortListener);
  obj->m_sessionPortListener.reset(new BusObject::SessionPortListener(*obj));
  DSB_ASSERT(!obj->m_busListener);
  obj->m_busListener.reset(new BusObject::BusListener(*obj));
  obj->m_bus->RegisterBusListener(*(obj->m_busListener.get()));

  st = obj->m_bus->Start();
  Error::ThrowIfNotOk(st, "failed to start bus attachment");

  st = obj->m_bus->Connect();
  Error::ThrowIfNotOk(st, "failed to connect to alljoyn router");
  DSBLOG_INFO("new bus connection: %s", obj->m_bus->GetConnectSpec().c_str());

  for (auto const& child : dev.GetChildren())
  {
    std::stringstream childPath;
    childPath << path;
    childPath << '/';
    childPath << child->GetInfo().GetName();

    std::shared_ptr<BusObject> childDevice(new BusObject(appname, childPath.str(), obj->m_bus));
    childDevice->m_adapter = adapter;
    childDevice->m_adapterDevice = *child;
    obj->m_children.push_back(childDevice);
  }

  return obj;
}

void
bridge::BusObject::Publish()
{
  PublishInternal(false);
  for (auto& child : m_children)
    child->PublishInternal(true);
  AnnounceAndRegister();
}

void
bridge::BusObject::PublishInternal(bool isChild)
{
  for (auto interface : m_adapterDevice.GetInterfaces())
  {
    ajn::InterfaceDescription const* intf = m_bus->GetInterface(interface.GetName().c_str());
    if (intf == nullptr)
      intf = BuildInterface(interface);

    if (intf != nullptr)
    {
      // DSBLOG_DEBUG("activating interface: %s", interface.GetName().c_str());
      ajn::BusObject::AddInterface(*intf, ajn::BusObject::ANNOUNCED);

      for (auto method : interface.GetMethods())
      {
        char const* name = method.GetName().c_str();

        // register method handler
        ajn::InterfaceDescription::Member const* member = intf->GetMember(name);
        if (member != nullptr)
        {
          // TODO: how to get rid of this cast?
          QStatus st = AddMethodHandler(member, static_cast<MessageReceiver::MethodHandler>(&BusObject::MethodHandler), this);
          Error::ThrowIfNotOk(st, "failed to register method handler for %s", name);
        }
      }
    }
  }

  if (!isChild)
  {
    ajn::SessionPort port = ajn::SESSION_PORT_ANY;
    ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);

    QStatus st = m_bus->BindSessionPort(port, opts, *(m_sessionPortListener.get()));
    Error::ThrowIfNotOk(st, "failed to bind session port");

    m_sessionPort = port;

    for (auto& child : m_children)
      child->m_sessionPort = port;
  }
}

void
bridge::BusObject::AnnounceAndRegister()
{
  adapter::ItemInformation const& devInfo = m_adapterDevice.GetInfo();

  // TODO: What AppId should we use?
  adapter::Guid appId = adapter::Guid::NewGuid();

  QStatus st = ER_OK;

  m_aboutData.reset(new ajn::AboutData("en"));
  st = m_aboutData->SetAppId(appId.ToByteArray());
  Error::ThrowIfNotOk(st, "failed to set appid");

  st = m_aboutData->SetAppName(m_adapter->GetApplicationName().c_str());
  Error::ThrowIfNotOk(st, "failed to set application name");

  st = m_aboutData->SetDeviceId((char const *)devInfo.GetId().ToByteArray());
  Error::ThrowIfNotOk(st, "failed to set device id");

  st = m_aboutData->SetDeviceName(devInfo.GetName().c_str());
  Error::ThrowIfNotOk(st, "failed to set device name");

  st = m_aboutData->SetManufacturer(devInfo.GetVendor().c_str());
  Error::ThrowIfNotOk(st, "failed to set manufacturer");

  st = m_aboutData->SetDescription(devInfo.GetDescription().c_str());
  Error::ThrowIfNotOk(st, "failed to set description");

  st = m_aboutData->SetDateOfManufacture("2016-01-01");
  Error::ThrowIfNotOk(st, "failed to set date of manufacture");

  st = m_aboutData->SetSoftwareVersion(devInfo.GetFirmwareVersion().c_str());
  Error::ThrowIfNotOk(st, "failed to set software version");

  st = m_aboutData->SetHardwareVersion(devInfo.GetVersion().c_str());
  Error::ThrowIfNotOk(st, "failed to set hardware version");

  st = m_aboutData->SetModelNumber(devInfo.GetModel().c_str());
  Error::ThrowIfNotOk(st, "failed to set model");

  m_aboutData->SetSupportUrl("http://www.xfinity.com");
  if (!m_aboutData->IsValid())
    Error::Throw("invalid about data");

  st = m_bus->RegisterBusObject(*this);
  Error::ThrowIfNotOk(st, "failed to register bus object");

  m_aboutObject.reset(new ajn::AboutObj(*m_bus.get()));
  DSBLOG_DEBUG("announce: %d", m_sessionPort);

  for (auto& child : m_children)
  {
    DSBLOG_INFO("register %s", child->GetPath());

    st = m_bus->RegisterBusObject(*child);
    Error::ThrowIfNotOk(st, "failed to register child object");
  }

  st = m_aboutObject->Announce(m_sessionPort, *m_aboutData.get());
  Error::ThrowIfNotOk(st, "failed to announce device");
}


QStatus
bridge::BusObject::Get(char const* interface, char const* property, ajn::MsgArg& arg)
{
  std::string ifc = interface;
  auto itr = s_interfaceCache.find(ifc);
  if (itr == s_interfaceCache.end())
  {
    DSBLOG_ERROR("can't find interface %s in cache", ifc.c_str());
    return ER_FAIL;
  }

  // find property
  adapter::Interface const& adapterInterface = *(itr->second);
  adapter::Property const*  adapterProperty = adapterInterface.FindProperty(property);
  adapter::Value            adapterValue;
  adapter::IoRequest::Pointer adapterRequest;
  adapter::Status st = m_adapter->GetProperty(m_adapterDevice, adapterInterface, *adapterProperty,
      adapterValue, adapterRequest);

  if (st == 0)
  {
    AllJoynHelper::ValueToMsgArg(adapterValue, arg);
  }

  return ER_OK;
}

QStatus
bridge::BusObject::Set(char const* interface, char const* property, ajn::MsgArg& msg)
{
  std::string ifc = interface;
  auto itr = s_interfaceCache.find(ifc);
  if (itr == s_interfaceCache.end())
  {
    DSBLOG_ERROR("can't find interface %s in cache", ifc.c_str());
    return ER_FAIL;
  }

  adapter::Interface const&   adapterInterface = *(itr->second);
  adapter::Property const*    adapterProperty = adapterInterface.FindProperty(property);
  adapter::Value              adapterValue;
  AllJoynHelper::MsgArgToValue(msg, adapterValue);

  adapter::IoRequest::Pointer adapterRequest;
  adapter::Status st = m_adapter->SetProperty(m_adapterDevice, adapterInterface, *adapterProperty,
    adapterValue, adapterRequest);

  if (st != 0)
  {
    DSBLOG_ERROR("failed to set property %s. %s", property, m_adapter->GetStatusText(st).c_str());
    return ER_FAIL;
  }

  return ER_OK;
}

bool
bridge::BusObject::SessionPortListener::AcceptSessionJoiner(ajn::SessionPort sessionPort,
  char const* joiner, ajn::SessionOpts const& /*opts*/)
{
  DSBLOG_INFO("accept session joiner [port:%d joiner:%s]", sessionPort, joiner);
  return true;
}

void
bridge::BusObject::SessionPortListener::SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id,
  char const* joiner)
{
  DSBLOG_INFO("session joined [port:%d id:0x%x joiner:%s]", sessionPort, id, joiner);
}

void
bridge::BusObject::SessionListener::SessionLost(ajn::SessionId sessionId, SessionLostReason reason)
{
  DSBLOG_INFO("session lost [id: 0x%x reason:%d]", sessionId, reason);
}

void
bridge::BusObject::SessionListener::SessionMemberAdded(ajn::SessionId sessionId, char const* uniqueName)
{
  DSBLOG_INFO("session member added [id:0x%x  name:%s]", sessionId, uniqueName);
}

void
bridge::BusObject::SessionListener::SessionMemberRemoved(ajn::SessionId sessionId, char const* uniqueName)
{
  DSBLOG_INFO("session member removed [id:0x%x name:%s]", sessionId, uniqueName);
}

void
bridge::BusObject::BusListener::BusDisconnected()
{
  DSBLOG_INFO("BusAttachment %s disconnected", m_parent.m_bus->GetGlobalGUIDString().c_str());

  // TODO: This is a hammer
  // m_parent.m_bus.reset(new ajn::BusAttachment(m_parent.m_appName.c_str(), true));
  // m_parent.m_bus->RegisterBusListener(*m_parent.m_busListener.get());
  // m_parent.m_bus->Start();

  std::unique_lock<std::mutex> lock(reconnectMutex);
  reconnectList.push_back(m_parent.m_bus.get());
  lock.unlock();

  reconnectCond.notify_one();
}

void
bridge::BusObject::MethodHandler(ajn::InterfaceDescription::Member const* member, ajn::Message& msg)
{
  if (!member)
    return;

  if (DSBLOG_LEVEL_ENABLED(Debug))
  {
    DSBLOG_DEBUG("MethodHandler\n"
      "\tsignature : '%s'\n"
      "\txml       : \n%s\n",
      msg->GetSignature(),
      msg->ToString().c_str());
  }

  std::string ifc = member->iface->GetName();
  auto itr = s_interfaceCache.find(ifc);
  if (itr == s_interfaceCache.end())
  {
    DSBLOG_ERROR("can't find interface %s in cache", ifc.c_str());
    return;
  }

  adapter::Interface const&   adapterInterface = *(itr->second);
  adapter::Method    const*   adapterMethod = adapterInterface.FindMethod(member->name.c_str());
  adapter::IoRequest::Pointer adapterRequest;
  adapter::Value              inarg;
  adapter::Value              outarg;
  adapter::Status st = m_adapter->InvokeMethod(m_adapterDevice, adapterInterface, *adapterMethod, inarg,
      outarg, adapterRequest);

  if (st == ER_OK)
  {
  }
  else
  {
    DSBLOG_WARN("failed to invoke method: %s", m_adapter->GetStatusText(st).c_str());
  }
}

void
bridge::BusObject::AddPropertiesToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc)
{
  for (auto prop : interface.GetProperties())
  {
    char const* name = prop.GetName().c_str();
    // DSBLOG_DEBUG("adding property to interface: %s", name);

    std::string sig = AllJoynHelper::GetSignature(prop.GetType());

    QStatus st = desc.AddProperty(name, sig.c_str(), GetAccess(prop));
    Error::ThrowIfNotOk(st, "failed to add property %s to interface %s", name,
        desc.GetName());

    for (auto attr : prop.GetAttributes())
    {
      st = desc.AddPropertyAnnotation(name, attr.first.c_str(), attr.second.ToString().c_str());
      Error::ThrowIfNotOk(st, "failed to add annotation to property");
    }
  }
}

void
bridge::BusObject::AddSignalsToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc)
{
  for (adapter::Signal const& signal : interface.GetSignals())
  {
    std::string const sig = AllJoynHelper::GetSignature(signal.GetParameters());
    char const* s = nullptr;
    if (!sig.empty())
      s = sig.c_str();

    std::string const names = AllJoynHelper::GetMethodArgumentNames(signal.GetParameters());
    char const* t = nullptr;
    if (!names.empty())
      t = names.c_str();

    QStatus st = desc.AddSignal(signal.GetName().c_str(), s, t, 0, nullptr);
    Error::ThrowIfNotOk(st, "failed to add signal");
  }
}

void
bridge::BusObject::AddMethodsToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc)
{
  for (auto method : interface.GetMethods())
  {
    char const* name = method.GetName().c_str();

    std::string osig = AllJoynHelper::GetSignature(method.GetOutputArguments());
    char const* outsig = nullptr;
    if (!osig.empty())
      outsig = osig.c_str();

    char const* insig = nullptr;
    std::string isig = AllJoynHelper::GetSignature(method.GetInputArguments());
    if (!isig.empty())
      insig = isig.c_str();

    std::string names = AllJoynHelper::GetMethodArgumentNames(method.GetInputArguments(),
        method.GetOutputArguments());

    QStatus st = desc.AddMethod(name, insig, outsig, names.c_str());
    Error::ThrowIfNotOk(st, "failed to add method %s", name);
  }
}

ajn::InterfaceDescription const*
bridge::BusObject::BuildInterface(adapter::Interface const& interface)
{
  QStatus st = ER_OK;
  ajn::InterfaceDescription* desc = nullptr;

  std::string ifcName = interface.GetName().c_str();

  {
    auto itr = s_interfaceCache.find(ifcName);
    if (itr == s_interfaceCache.end())
    {
      s_interfaceCache.insert(InterfaceCache::value_type(
            ifcName, std::shared_ptr<adapter::Interface>(new adapter::Interface(interface))));
    }
  }

  // DSBLOG_DEBUG("looking for interface: %s", interface.GetName().c_str());

  st = m_bus->CreateInterface(interface.GetName().c_str(), desc, ajn::AJ_IFC_SECURITY_OFF);
  Error::ThrowIfNotOk(st, "failed to create interface: %s", interface.GetName().c_str());

  for (auto attr : interface.GetAttributes())
  {
    // DSBLOG_DEBUG("adding annotation to interface");
    st = desc->AddAnnotation(attr.first.c_str(), attr.second.ToString().c_str());
    Error::ThrowIfNotOk(st, "failed to add annotation to interface");
  }

  AddPropertiesToInterface(interface, *desc);
  AddMethodsToInterface(interface, *desc);
  AddSignalsToInterface(interface, *desc);

  desc->Activate();

  return m_bus->GetInterface(interface.GetName().c_str());
}
