
#include "Adapters/MockAdapter/MockAdapter.h"

#include "Common/AdapterProperty.h"
#include "Common/AdapterLog.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

namespace
{
  DSB_DECLARE_LOGNAME(MockAdapter);
  
  std::map< uint64_t, adapter::Value > valueCache;

  uint16_t GetAttributeId(adapter::Property const& p)
  {
    adapter::Value v = p.GetAttribute("code");
    return v.ToUInt16();
  }

  uint16_t GetClusterId(adapter::Interface const& i)
  {
    adapter::Value v = i.GetAttribute("code");
    return v.ToUInt16();
  }

  void AddClusterToDevice(adapter::Device& dev, uint16_t clusterId)
  {
    std::shared_ptr<adapter::Interface> def = adapters::mock::GetZigBeeCluster(clusterId);
    if (def)
      dev.AddInterface(*def);
  }
}

adapters::mock::MockAdapter::MockAdapter()
  : m_exposedApplicationName("DeviceSystemBridge")
  , m_exposedApplicationGuid(adapter::Guid::Parse("C27BC425-0058-4829-8775-441B5D8740C0"))
{
  m_info.SetName("MockAdapter");
  m_info.SetVendor("Comcast");
  m_exposedAdapterPrefix = "/zigbee";

  // TODO: get m_exposedApplicatioName and Prefix from config
}

adapters::mock::MockAdapter::~MockAdapter()
{
  Shutdown(std::shared_ptr<adapter::IoRequest>());
}

std::string
adapters::mock::MockAdapter::GetAdapterPrefix()
{
  return m_exposedAdapterPrefix;
}

std::string
adapters::mock::MockAdapter::GetApplicationName()
{
  return m_exposedApplicationName;
}

adapter::Guid
adapters::mock::MockAdapter::GetApplicationGuid()
{
  return m_exposedApplicationGuid;
}

adapter::Status
adapters::mock::MockAdapter::GetSignals(
  adapter::Signal::Vector& signals,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  DSBLOG_ASSERT_NOT_IMPLEMENTED();
  return 0;
}

adapter::Status
adapters::mock::MockAdapter::Initialize(
  std::shared_ptr<adapter::Log> const& log,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  m_log = log;

  CreateMockDevices();

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::Shutdown(std::shared_ptr<adapter::IoRequest> const& req)
{
  m_exposedAdapterPrefix.clear();
  m_exposedApplicationName.clear();
  m_devices.clear();
  m_signals.clear();
  m_signalListeners.clear();

  if (req)
    req->SetComplete(0);

  return 0;
}


adapter::Status
adapters::mock::MockAdapter::EnumDevices(
    adapter::EnumDeviceOptions /*opts*/,
    adapter::Device::Vector& devices,
    std::shared_ptr<adapter::IoRequest> const& req)
{
  devices.clear();
  std::copy(m_devices.begin(), m_devices.end(), std::back_inserter(devices));

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::GetProperty(
  adapter::Device const& device,
  adapter::Interface const& ifc,
  adapter::Property const& prop,
  adapter::Value& value,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  uint64_t id = prop.GetId();

  auto itr = valueCache.find(id);
  if (itr != valueCache.end())
  {
    value = itr->second;
  }
  else
  {
    adapter::Value v = prop.GetAttribute("default");
    if (!v.IsEmpty())
    {
      value = v;
    }
    else
    {
      // use default for type
      adapter::TypeId type = prop.GetType();
      value = adapter::Value(adapter::Value(type));
    }
  }

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::SetProperty(
  adapter::Device const& device,
  adapter::Interface const& ifc,
  adapter::Property const& prop,
  adapter::Value const& value,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  DSBLOG_INFO("SetProperty %s, %s", ifc.GetName().c_str(), prop.GetName().c_str());

  // you put this in during the modeling phase
  uint16_t clusterId = GetClusterId(ifc);
  uint16_t attrId = GetAttributeId(prop);

  DSBLOG_INFO("set cluster:0x%02x attr:0x%02x", clusterId, attrId);

  // TODO: I think we make the bridge do the type checking, but the adapter would
  // have to do range checking
  valueCache[prop.GetId()] = value;

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::InvokeMethod(
  adapter::Device const& device,
  adapter::Interface const& ifc,
  adapter::Method const& method,
  adapter::Value const& inarg,
  adapter::Value& ourarg,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  DSBLOG_INFO("TODO: InvokeMethod: %s.%s", ifc.GetName().c_str(), method.GetName().c_str());
  return 0;
}


adapter::Status
adapters::mock::MockAdapter::RegisterSignalListener(
  adapter::Device const& device,
  adapter::Interface const& interface,
  adapter::Signal const& signal,
  adapter::SignalListener const& listener,
  void* argp,
  adapter::RegistrationHandle& handle)
{
  DSBLOG_ASSERT_NOT_IMPLEMENTED();
#if 0
  RegisteredSignal reg;
  reg.Listener = listener;
  reg.Context = argp;
  reg.RegHandle = GetNextRegistrationHandle();
  m_signalListeners.insert(SignalMap::value_type(signalName, reg));
  handle = reg.RegHandle;
#endif
  return 0;
}

adapter::Status
adapters::mock::MockAdapter::UnregisterSignalListener(adapter::RegistrationHandle const& h)
{
  DSBLOG_ASSERT_NOT_IMPLEMENTED();

  adapter::Status st = 1;
  for (SignalMap::iterator begin = m_signalListeners.begin(), end = m_signalListeners.end();
    begin != end; ++begin)
  {
    if (begin->second.RegHandle == h)
    {
      m_signalListeners.erase(begin);
      st = 0;
      break;
    }
  }
  return st;
}

void
adapters::mock::MockAdapter::CreateMockDevices()
{
  m_devices.push_back(MockAdapter::CreateDoorWindowSensor("FrontDoor"));
  m_devices.push_back(MockAdapter::CreateDoorWindowSensor("SideDoor"));
  m_devices.push_back(MockAdapter::CreateDoorWindowSensor("RearDoor"));
}

std::string
adapters::mock::MockAdapter::GetStatusText(adapter::Status st)
{
  if (st == 0)
    return std::string("ok");

  std::stringstream buff;
  buff << "MockError:";
  buff << static_cast<uint32_t>(st);
  return buff.str();
}

adapter::ItemInformation const&
adapters::mock::MockAdapter::GetInfo()
{
  return m_info;
}

adapter::Status
adapters::mock::MockAdapter::GetConfiguration(
    std::vector<uint8_t>& configData,
    std::shared_ptr<adapter::IoRequest> const& req)
{
  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::SetConfiguration(
    std::vector<uint8_t> const& configData,
    std::shared_ptr<adapter::IoRequest> const& req)
{
  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Device
adapters::mock::MockAdapter::CreateDoorWindowSensor(std::string const& name)
{
  adapter::ItemInformation info;
  info.SetName(name);
  info.SetVendor("Comcast");
  info.SetModel("801417");
  info.SetVersion("1.0");
  info.SetFirmwareVersion("1.0");
  info.SetSerialNumber("H123ALK");
  info.SetDescription("Door/Window sensor");

  adapter::Device dev;
  dev.SetBasicInfo(info);

  AddClusterToDevice(dev, 0x0000); // basic
  AddClusterToDevice(dev, 0x000f); // binary input

  return std::move(dev);
}
