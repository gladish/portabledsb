#include "MockAdapter.h"

#include "MockDevices.h"
#include "MockAdapterDevice.h"
#include "Common/Log.h"

#include <vector>
#include <algorithm>

namespace
{
  DSB_DECLARE_LOGNAME(MockAdapter);
  bridge::IAdapter::RegistrationHandle nextHandle = 0;
  bridge::IAdapter::RegistrationHandle GetNextRegistrationHandle()
  {
    return ++nextHandle;
  }
}

adapters::mock::MockAdapter::MockAdapter()
  : m_vendor("Acme")
  , m_adapterName("DSB Mock Adapter")
  , m_exposedAdapterPrefix("com." + m_vendor)
  , m_exposedApplicationName("DeviceSystemBridge")
  , m_exposedApplicationGuid("C27BC425-0058-4829-8775-441B5D8740C0")
{
  // TODO: get m_exposedApplicatioName and Prefix from config
}

adapters::mock::MockAdapter::~MockAdapter()
{
  Shutdown();
}

std::string
adapters::mock::MockAdapter::GetVendor()
{
  return m_vendor;
}

std::string
adapters::mock::MockAdapter::GetAdapterName()
{
  return m_adapterName;
}

std::string
adapters::mock::MockAdapter::GetVersion()
{
  return m_version;
}

std::string
adapters::mock::MockAdapter::GetExposedAdapterPrefix()
{
  return m_exposedAdapterPrefix;
}

std::string
adapters::mock::MockAdapter::GetExposedApplicationName()
{
  return m_exposedApplicationName;
}

std::string
adapters::mock::MockAdapter::GetExposedApplicationGuid()
{
  return m_exposedApplicationGuid;
}

bridge::AdapterSignalVector adapters::mock::MockAdapter::GetSignals()
{
  return bridge::AdapterSignalVector();
}


int32_t
adapters::mock::MockAdapter::Initialize()
{
  CreateMockDevices();
  CreateSignals();
  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::Shutdown()
{
  m_vendor.clear();
  m_adapterName.clear();
  m_version.clear();
  m_exposedAdapterPrefix.clear();
  m_exposedApplicationName.clear();
  m_exposedApplicationGuid.clear();

  m_devices.clear();
  m_signals.clear();

  m_signalListeners.clear();
  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::EnumDevices(
    bridge::EnumDeviceOptions /*opts*/,
    bridge::AdapterDeviceVector& deviceList,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req->reset();
    

  deviceList.clear();
  std::copy(m_devices.begin(), m_devices.end(), std::back_inserter(deviceList));

  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::GetProperty(
    shared_ptr<bridge::IAdapterProperty>& prop,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req->reset();

  shared_ptr<MockAdapterProperty> mockAdapterProperty = dynamic_pointer_cast<MockAdapterProperty>(prop);
  if (!mockAdapterProperty)
    return ER_BAD_ARG_1;

  shared_ptr<MockAdapterDevice> device = mockAdapterProperty->GetParent();
  if (!device)
    return ER_INVALID_ADDRESS;

  shared_ptr<MockAdapter> adapter = device->GetParent();
  if (!adapter)
    return ER_INVALID_ADDRESS;

  // TODO: set prop with latest stuff
  
  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::SetProperty(
    shared_ptr<bridge::IAdapterProperty> const& prop,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req ->reset();

  shared_ptr<MockAdapterProperty> mockAdapterProperty = dynamic_pointer_cast<MockAdapterProperty>(prop);
  if (!mockAdapterProperty)
    return ER_BAD_ARG_1;

  shared_ptr<MockAdapterDevice> device = mockAdapterProperty->GetParent();
  if (!device)
    return ER_INVALID_ADDRESS;

  shared_ptr<MockAdapter> adapter = device->GetParent();
  if (!adapter)
    return ER_INVALID_ADDRESS;

  // TODO: set internal state from prop->GetData()
  
  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::GetPropertyValue(
    shared_ptr<bridge::IAdapterProperty> const& prop,
    std::string const& attributeName,
    shared_ptr<bridge::IAdapterValue>& value,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req->reset();

  shared_ptr<MockAdapterProperty> mockAdapterProperty = dynamic_pointer_cast<MockAdapterProperty>(prop);
  if (!mockAdapterProperty)
    return ER_BAD_ARG_1;

  shared_ptr<MockAdapterValue> attr = mockAdapterProperty->GetAttributeByName(attributeName);
  if (!attr)
    return ER_INVALID_ADDRESS;

  value = dynamic_pointer_cast<bridge::IAdapterValue>(attr);

  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::SetPropertyValue(
    shared_ptr<bridge::IAdapterProperty> const& prop,
    shared_ptr<bridge::IAdapterValue> const& value,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req->reset(); 

  shared_ptr<MockAdapterProperty> mockAdapterProperty = dynamic_pointer_cast<MockAdapterProperty>(prop);
  if (!mockAdapterProperty)
    return ER_BAD_ARG_1;

  shared_ptr<MockAdapterValue> attr = mockAdapterProperty->GetAttributeByName(value->GetName());
  if (!attr)
    return ER_INVALID_ADDRESS;

  attr->SetData(value->GetData());

  return ER_OK;
}


int32_t
adapters::mock::MockAdapter::CallMethod(
    shared_ptr<bridge::IAdapterMethod>& method,
    shared_ptr<bridge::IAdapterIoRequest>* req)
{
  shared_ptr<MockAdapterMethod> m = dynamic_pointer_cast<MockAdapterMethod>(method);
  if (!m)
    return ER_BAD_ARG_1;

  shared_ptr<MockAdapterDevice> device = m->GetParent();
  if (!device)
    return ER_INVALID_ADDRESS;

  return device->DispatchMethod(m, req);
}


int32_t
adapters::mock::MockAdapter::RegisterSignalListener(
    std::string const& signalName,
    shared_ptr<bridge::IAdapterSignalListener> const& listener,
    void* argp,
    bridge::IAdapter::RegistrationHandle& handle)
{
  RegisteredSignal reg;
  reg.Listener = listener;
  reg.Context = argp;
  reg.RegHandle = GetNextRegistrationHandle();
  m_signalListeners.insert(SignalMap::value_type(signalName, reg));
  handle = reg.RegHandle;
  return ER_OK;
}

int32_t
adapters::mock::MockAdapter::UnregisterSignalListener(bridge::IAdapter::RegistrationHandle const& h)
{
  int32_t st = ER_FAIL;
  for (SignalMap::iterator begin = m_signalListeners.begin(), end = m_signalListeners.end();
    begin != end; ++begin)
  {
    if (begin->second.RegHandle == h)
    {
      m_signalListeners.erase(begin);
      st = ER_OK;
      break;
    }
  }
  return st;
}

void
adapters::mock::MockAdapter::CreateMockDevices()
{
  shared_ptr<MockAdapter> self = dynamic_pointer_cast<MockAdapter>(shared_from_this());

  typedef std::vector<adapters::mock::MockDeviceDescriptor> vector;

  vector devices = adapters::mock::GetMockDevices();
  for (vector::const_iterator begin = devices.begin(), end = devices.end();
    begin != end; ++begin)
  {
    shared_ptr<MockAdapterDevice> dev = MockAdapterDevice::Create(*begin, self);
    m_devices.push_back(dev);
  }
}

void
adapters::mock::MockAdapter::CreateSignals()
{
  shared_ptr<MockAdapterDevice> parent;

  bridge::AdapterValueVector values;

  shared_ptr<MockAdapterValue> val(new MockAdapterValue(bridge::kDeviceArravalHandle));
  shared_ptr<MockAdapterSignal> signal(new MockAdapterSignal(bridge::kDeviceArrivalSignal,
    parent, values));
  m_signals.push_back(signal);
}

int32_t
adapters::mock::MockAdapter::NotifySignalListeners(shared_ptr<MockAdapterSignal> const& signal)
{
  int32_t st = ER_FAIL;
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
}

