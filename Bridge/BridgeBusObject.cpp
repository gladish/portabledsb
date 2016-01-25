#include "Bridge/BridgeBusObject.h"
#include "Bridge/BridgeError.h"
#include "Bridge/AllJoynHelper.h"
#include "Common/AdapterLog.h"

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

  ajn::InterfaceDescription const* BuildInterface(ajn::BusAttachment& bus, adapter::Interface const& interface)
  {
    QStatus st = ER_OK;
    ajn::InterfaceDescription* intf = nullptr;

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
    
    st = bus.CreateInterface(interface.GetName().c_str(), intf, ajn::AJ_IFC_SECURITY_OFF);
    bridge::Error::ThrowIfNotOk(st, "failed to create interface: %s", interface.GetName().c_str());

    for (auto attr : interface.GetAttributes())
    {
      // DSBLOG_DEBUG("adding annotation to interface");
      st = intf->AddAnnotation(attr.first.c_str(), attr.second.ToString().c_str());
      bridge::Error::ThrowIfNotOk(st, "failed to add annotation to interface");
    }

    for (auto prop : interface.GetProperties())
    {
      char const* name = prop.GetName().c_str();
      // DSBLOG_DEBUG("adding property to interface: %s", name);

      std::string sig = bridge::AllJoynHelper::GetSignature(prop.GetType());

      st = intf->AddProperty(name, sig.c_str(), GetAccess(prop));
      bridge::Error::ThrowIfNotOk(st, "failed to add property %s to interface %s", name, ifcName.c_str());

      for (auto attr : prop.GetAttributes())
      {
        st = intf->AddPropertyAnnotation(name, attr.first.c_str(), attr.second.ToString().c_str());
        bridge::Error::ThrowIfNotOk(st, "failed to add annotation to property");
      }
    }

    for (auto method : interface.GetMethods())
    {
      // TODO:
    }

    for (auto signal : interface.GetSignals())
    {
      // TODO:
    }

    if (intf != nullptr)
      intf->Activate();

    return bus.GetInterface(interface.GetName().c_str());
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

  return obj;
}

void
bridge::BusObject::Publish()
{
  for (auto interface : m_adapterDevice.GetInterfaces())
  {
    ajn::InterfaceDescription const* intf = m_bus->GetInterface(interface.GetName().c_str());
    if (intf == nullptr)
      intf = BuildInterface(*m_bus.get(), interface);

    if (intf != nullptr)
    {
      // DSBLOG_DEBUG("activating interface: %s", interface.GetName().c_str());
      ajn::BusObject::AddInterface(*intf, ajn::BusObject::ANNOUNCED);
    }
  }

  ajn::SessionPort port = ajn::SESSION_PORT_ANY;
  ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);

  QStatus st = m_bus->BindSessionPort(port, opts, *(m_sessionPortListener.get()));
  Error::ThrowIfNotOk(st, "failed to bind session port");

  m_sessionPort = port;;

  AnnounceAndRegister();
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

  st = m_aboutObject->Announce(m_sessionPort, *m_aboutData.get());
  Error::ThrowIfNotOk(st, "failed to announce device");
}


// yes just testing
static ajn::MsgArg s_val("s", "This is the default description");

QStatus
bridge::BusObject::Get(char const* interface, char const* property, ajn::MsgArg &val)
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
  adapter::Property const*  adapterProperty = adapterInterface.GetProperty(property);
  adapter::Value            adapterValue;
  adapter::IoRequest::Pointer adapterRequest;
  adapter::Status st = m_adapter->GetProperty(adapterInterface, *adapterProperty,
      adapterValue, adapterRequest);

  if (st == 0)
  {
    AllJoynHelper::SetMsgArg(adapterValue, val);
    printf("ValueType: %d\n", adapterValue.GetValue().GetType());
  }

  return ER_OK;
}

QStatus
bridge::BusObject::Set(char const* interface, char const* property, ajn::MsgArg &val)
{
  std::string ifc = interface;
  auto itr = s_interfaceCache.find(ifc);
  if (itr == s_interfaceCache.end())
  {
    DSBLOG_ERROR("can't find interface %s in cache", ifc.c_str());
    return ER_FAIL;
  }

  adapter::Interface const&   adapterInterface = *(itr->second);
  adapter::Property const*    adapterProperty = adapterInterface.GetProperty(property);

  adapter::Value              adapterValue(property, 0);
  AllJoynHelper::GetValue(adapterValue, val);

  adapter::IoRequest::Pointer adapterRequest;
  adapter::Status st = m_adapter->SetProperty(adapterInterface, *adapterProperty,
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

