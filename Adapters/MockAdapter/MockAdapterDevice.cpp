#include "Adapters/MockAdapter/MockAdapterDevice.h"
#include "Adapters/MockAdapter/MockAdapter.h"

using namespace adapters::mock;

shared_ptr<MockAdapterDevice> MockAdapterDevice::Create(
    MockDeviceDescriptor const& desc,
    weak_ptr<MockAdapter> const& parent)
{
  shared_ptr<MockAdapterDevice> device(new MockAdapterDevice(desc, parent));

  for (int i = 0; i < kMockDeviceMaxProperties; ++i)
  {
    MockPropertyDescriptor const& descriptor = desc.PropertyDescriptors[i];
    if (descriptor.Id == kLastDescriptorId)
      break;

    device->m_properties.push_back(shared_ptr<bridge::IAdapterProperty>(
          new MockAdapterProperty(descriptor, device)));
  }

  device->CreateMethods();
  device->CreateSignals();
  
  return device;
}

MockAdapterDevice::MockAdapterDevice(MockDeviceDescriptor const& desc, weak_ptr<MockAdapter> const& parent)
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
  m_signalPrototypes.push_back(shared_ptr<MockAdapterSignal>(new MockAdapterSignal(
          bridge::kChangeOfValueSignal, shared_from_this(), params)));

  // TODO: This is incomplete, re-visit once signal emit is worked on 
}

void MockAdapterDevice::CreateMethods()
{
  shared_ptr<MockAdapterDevice> self = shared_from_this();

  shared_ptr<MockAdapterMethod> m(new MockAdapterMethod(kDeviceResetMethod, self));
  m->AddInputParam(shared_ptr<bridge::IAdapterValue>(new MockAdapterValue(kDeviceResetPropertyHandle)));
  m_methods.push_back(m);
}

int32_t MockAdapterDevice::DispatchMethod(
  shared_ptr<MockAdapterMethod>& method,
  shared_ptr<bridge::IAdapterIoRequest>* req)
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
    shared_ptr<MockAdapterProperty> const& prop,
    shared_ptr<MockAdapterValue> const& value)
{
  if (!prop)
    return ER_BAD_ARG_2;
  if (!value)
    return ER_BAD_ARG_3;

  int32_t st = ER_FAIL;

  shared_ptr<MockAdapterSignal> signal;
  shared_ptr<MockAdapter> parent = m_parent.lock();
  if (!parent)
    return ER_FAIL;

  typedef bridge::AdapterSignalVector::const_iterator iterator;
  for (iterator begin = m_signalPrototypes.begin(), end = m_signalPrototypes.end();
    ((begin != end) && !signal); ++begin)
  {
    if ((*begin)->GetName() == signalName)
    {
      signal = dynamic_pointer_cast<MockAdapterSignal>(*begin);
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

    shared_ptr<MockAdapterDevice> self = shared_from_this();

    shared_ptr<MockAdapterValue> p(new MockAdapterValue("PropertyName"));
    p->SetData(common::Variant(prop->GetName()));

    shared_ptr<MockAdapterValue> v(new MockAdapterValue("PropertyValue"));
    v->SetData(value->GetData());

    params.push_back(p);
    params.push_back(v);
  }

  if (signal)
    parent->NotifySignalListeners(signal);

  return st;
}

int32_t MockAdapterDevice::Reset(shared_ptr<MockAdapterMethod>& method)
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
    shared_ptr<bridge::IAdapterValue> arg1 = inParams[0];
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

shared_ptr<bridge::IAdapterIcon> MockAdapterDevice::GetIcon()
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

MockAdapterProperty::MockAdapterProperty( MockPropertyDescriptor const& desc,
  weak_ptr<MockAdapterDevice> const& parent)
  : m_name(desc.Name)
  , m_parent(parent)
{
}

std::string MockAdapterProperty::GetName()
{
  return m_name;
}

shared_ptr<MockAdapterValue> MockAdapterProperty::GetAttributeByName(std::string const& name)
{
  typedef bridge::AdapterValueVector::const_iterator iterator;
  for (iterator begin = m_attributes.begin(), end = m_attributes.end(); begin != end; ++begin)
  {
    if ((*begin)->GetName() == name)
      return dynamic_pointer_cast<MockAdapterValue>(*begin);
  }
  return shared_ptr<MockAdapterValue>();
}


bridge::AdapterValueVector MockAdapterProperty::GetAttributes()
{
  return m_attributes;
}



// MockAdapterMethod 
MockAdapterMethod::MockAdapterMethod(
      std::string const& name,
      weak_ptr<MockAdapterDevice> const& parent)
  : m_name(name)
  , m_parent(parent)
{
}

std::string MockAdapterMethod::GetName() const
{
  return m_name;
}

std::string MockAdapterMethod::GetDescription()
{
  return m_description;
}

bridge::AdapterValueVector const& MockAdapterMethod::GetInputParams() const
{
  return m_inputParams;
}

bridge::AdapterValueVector const& MockAdapterMethod::GetOutputParams() const
{
  return m_outputParams;
}

void MockAdapterMethod::SetInputParams(bridge::AdapterValueVector const& params)
{
  m_inputParams = params;
}

void MockAdapterMethod::SetOutputParams(bridge::AdapterValueVector const& params)
{
  m_outputParams = params;
}

void MockAdapterMethod::AddInputParam(shared_ptr<bridge::IAdapterValue> const& p)
{
  m_inputParams.push_back(p);
}

void MockAdapterMethod::AddOutputParam(shared_ptr<bridge::IAdapterValue> const& p)
{
  m_outputParams.push_back(p);
}

int32_t MockAdapterMethod::GetResult()
{
  return m_result;
}

void MockAdapterMethod::SetResult(int32_t st)
{
  m_result = st;
}

MockAdapterValue::MockAdapterValue(std::string const& name)
  : m_name(name)
{
}


std::string MockAdapterValue::GetName()
{
  return m_name;
}

common::Variant const& MockAdapterValue::GetData() const
{
  return m_data;
}

void MockAdapterValue::SetData(common::Variant const& v)
{
  m_data = v;
}

MockAdapterSignal::MockAdapterSignal(
      std::string const& name,
      weak_ptr<MockAdapterDevice> const& parent,
      bridge::AdapterValueVector const& params)
  : m_name(name)
  , m_parent(parent)
  , m_params(params)
{
}

shared_ptr<MockAdapterSignal> MockAdapterSignal::Clone()
{
  shared_ptr<MockAdapterSignal> clone(new MockAdapterSignal(
    m_name,
    m_parent,
    m_params));
  return clone;
}


std::string MockAdapterSignal::GetName() const
{
  return m_name;
}

bridge::AdapterValueVector const& MockAdapterSignal::GetParams() const
{
  return m_params;
}


