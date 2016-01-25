
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
  
  adapter::RegistrationHandle nextHandle = 0;
  adapter::RegistrationHandle GetNextRegistrationHandle()
  {
    return ++nextHandle;
  }

  std::map< uint64_t, adapter::Value > valueCache;
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
  CreateSignals();

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
    adapter::Variant v = prop.GetAttribute("default");
    if (!v.IsEmpty())
    {
      value = adapter::Value(prop.GetName(), v);
    }
    else
    {
      // use default for type
      adapter::TypeId type = prop.GetType();
      value = adapter::Value(prop.GetName(), adapter::Variant(type));
    }
  }

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::SetProperty(
  adapter::Interface const& ifc,
  adapter::Property const& prop,
  adapter::Value const& value,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  DSBLOG_INFO("SetProperty %s, %s", ifc.GetName().c_str(), prop.GetName().c_str());

  // TODO: I think we make the bridge do the type checking, but the adapter would
  // have to do range checking
  valueCache[prop.GetId()] = value;

  if (req)
    req->SetComplete(0);

  return 0;
}

adapter::Status
adapters::mock::MockAdapter::InvokeMethod(
  adapter::Interface const& ifc,
  adapter::Method const& method,
  std::shared_ptr<adapter::IoRequest> const& req)
{
  DSBLOG_ASSERT_NOT_IMPLEMENTED();
  return 0;
}


adapter::Status
adapters::mock::MockAdapter::RegisterSignalListener(
    std::string const& signalName,
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

#if 0
  std::shared_ptr<MockAdapter> self = std::dynamic_pointer_cast<MockAdapter>(shared_from_this());

  typedef std::vector<adapters::mock::MockDeviceDescriptor> vector;

  vector devices = adapters::mock::GetMockDevices();
  for (vector::const_iterator begin = devices.begin(), end = devices.end();
    begin != end; ++begin)
  {
    std::shared_ptr<MockAdapterDevice> dev = MockAdapterDevice::Create(*begin, self);
    m_devices.push_back(dev);
  }
#endif
}

void
adapters::mock::MockAdapter::CreateSignals()
{
  DSBLOG_NOT_IMPLEMENTED();
#if 0
  std::shared_ptr<MockAdapterDevice> parent;

  adapter::Value::Vector values;

  // TODO
  // std::shared_ptr<MockAdapterValue> val(new MockAdapterValue(bridge::kDeviceArravalHandle));
  // std::shared_ptr<MockterSignal> signal(new MockAdapterSignal(bridge::kDeviceArrivalSignal,
  //   parent, values));
  // m_signals.push_back(signal);
#endif
}

adapter::Status
adapters::mock::MockAdapter::NotifySignalListeners(adapter::Signal const& signal)
{
#if 0
  adapter::Status st = 1;
  if (!signal)
    return st;

  std::pair< SignalMap::iterator, SignalMap::iterator > range = 
    m_signalListeners.equal_range(signal->GetName());

  for (SignalMap::iterator begin = range.first, end = range.second; begin != end; ++begin)
  {
    RegisteredSignal& handler = begin->second;
    handler.Listener->AdapterSignalHandler(*signal, handler.Context);
  }

  return st;
  #endif
  return 0;
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

