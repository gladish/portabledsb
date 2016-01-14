#include "Adapters/MockAdapter/MockAdapterDevice.h"
#include "Adapters/MockAdapter/MockAdapterSignal.h"
#include "Adapters/MockAdapter/MockAdapter.h"

using namespace adapters::mock;

std::shared_ptr<MockAdapterDevice> MockAdapterDevice::Create(
    MockDeviceDescriptor const& desc,
    std::weak_ptr<MockAdapter> const& parent)
{
  std::shared_ptr<MockAdapterDevice> device(new MockAdapterDevice(desc, parent));

  for (int i = 0; i < kMockDeviceMaxProperties; ++i)
  {
    MockPropertyDescriptor const& descriptor = desc.PropertyDescriptors[i];
    if (descriptor.Id == kLastDescriptorId)
      break;

    device->m_properties.push_back(std::shared_ptr<bridge::IAdapterProperty>(
          new MockAdapterProperty(descriptor, device)));
  }

  device->CreateMethods();
  device->CreateSignals();
  
  return device;
}

MockAdapterDevice::MockAdapterDevice(MockDeviceDescriptor const& desc, std::weak_ptr<MockAdapter> const& parent)
  : m_name(desc.Name)
  , m_parent(parent)
  , m_vendor(desc.VendorName)
  , m_model(desc.Model)
  , m_version("never set") // TODO
  , m_firmwareVersion(desc.Version)
  , m_serialNumber(desc.SerialNumber)
  , m_description(desc.Description)
{

}

void MockAdapterDevice::CreateSignals()
{
  bridge::AdapterValueVector params;
  m_signalPrototypes.push_back(std::shared_ptr<MockAdapterSignal>(new MockAdapterSignal(
          bridge::kChangeOfValueSignal, shared_from_this(), params)));

  // TODO: This is incomplete, re-visit once signal emit is worked on 
}

void MockAdapterDevice::CreateMethods()
{
  std::shared_ptr<MockAdapterDevice> self = shared_from_this();

  std::shared_ptr<MockAdapterMethod> m(new MockAdapterMethod(kDeviceResetMethod, self));
  m->AddInputParam(std::shared_ptr<bridge::IAdapterValue>(new MockAdapterValue(kDeviceResetPropertyHandle)));
  m_methods.push_back(m);
}

int32_t MockAdapterDevice::DispatchMethod(
  std::shared_ptr<MockAdapterMethod>& method,
  std::shared_ptr<bridge::IAdapterIoRequest>* req)
{
  if (req)
    req->reset();

  int32_t st = ER_OK;
  if (method->GetName() == kDeviceResetMethod)
    st = Reset(method);

  return st;
}

int32_t MockAdapterDevice::SendSignal(
    std::string const& signalName,
    std::shared_ptr<MockAdapterProperty> const& prop,
    std::shared_ptr<MockAdapterValue> const& value)
{
  if (!prop)
    return ER_BAD_ARG_2;
  if (!value)
    return ER_BAD_ARG_3;

  int32_t st = ER_FAIL;

  std::shared_ptr<MockAdapterSignal> signal;
  std::shared_ptr<MockAdapter> parent = m_parent.lock();
  if (!parent)
    return ER_FAIL;

  typedef bridge::AdapterSignalVector::const_iterator iterator;
  for (iterator begin = m_signalPrototypes.begin(), end = m_signalPrototypes.end();
    ((begin != end) && !signal); ++begin)
  {
    if ((*begin)->GetName() == signalName)
    {
      signal = std::dynamic_pointer_cast<MockAdapterSignal>(*begin);
      DSB_ASSERT(signal != NULL);
      if (signal)
        signal = signal->Clone();
    }
  }

  if (!signal)
    return st;

  if (signal->GetName() == bridge::kChangeOfValueSignal)
  {
    bridge::AdapterValueVector params;

    std::shared_ptr<MockAdapterDevice> self = shared_from_this();

    std::shared_ptr<MockAdapterValue> p(new MockAdapterValue("PropertyName"));
    p->SetData(common::Variant(prop->GetName()));

    std::shared_ptr<MockAdapterValue> v(new MockAdapterValue("PropertyValue"));
    v->SetData(value->GetData());

    params.push_back(p);
    params.push_back(v);
  }

  if (signal)
    parent->NotifySignalListeners(signal);

  return st;
}

int32_t MockAdapterDevice::Reset(std::shared_ptr<MockAdapterMethod>& method)
{
  int32_t st = ER_OK;

  bridge::AdapterValueVector const& inParams = method->GetInputParams();
  if (inParams.size() == 0)
  {
    // reset device
  }
  else if (inParams.size() == 1)
  {
    // reset property by name
    std::shared_ptr<bridge::IAdapterValue> arg1 = inParams[0];
    if (!arg1)
      return ER_BAD_ARG_1;
  }

  method->SetResult(ER_OK);

  return st;
}

std::string MockAdapterDevice::GetName()
{
  return m_name;
}

std::string MockAdapterDevice::GetVendor()
{
  return m_vendor;
}

std::string MockAdapterDevice::GetModel()
{
  return m_model;
}

std::string MockAdapterDevice::GetVersion()
{
  return m_version;
}

std::string MockAdapterDevice::GetFirmwareVersion()
{
  return m_firmwareVersion;
}

std::string MockAdapterDevice::GetSerialNumber()
{
  return m_serialNumber;
}

std::string MockAdapterDevice::GetDescription()
{
  return m_description;
}

std::shared_ptr<bridge::IAdapterIcon> MockAdapterDevice::GetIcon()
{
  return m_icon;
}

bridge::AdapterPropertyVector const& MockAdapterDevice::GetProperties() const
{
  return m_properties;
}

bridge::AdapterMethodVector const& MockAdapterDevice::GetMethods() const
{
  return m_methods;
}

bridge::AdapterSignalVector const& MockAdapterDevice::GetSignals() const
{
  return m_signalPrototypes;
}
