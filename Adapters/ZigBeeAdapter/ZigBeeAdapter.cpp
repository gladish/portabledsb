#include "Adapters/ZigBeeAdapter/ZigBeeAdapter.h"

namespace
{
  std::string const kVendor("Charlie Root");
  std::string const kAdapterName("ZigBeeAdapter");
  std::string const kVersion("1.0");
  std::string const kExposedPrefix("com.allseen");
  std::string const kExposedAppName("The ZigBee Adapter");
  common::Guid const kExposedGuid = common::Guid::Parse("B8D50823-9F64-4110-AA0C-3CB3D17B73F2");
}

adapter::zigbee::Adapter::Adapter()
{
}

adapter::zigbee::Adapter::~Adapter()
{
}

std::string
adapter::zigbee::Adapter::GetVendor()
{
  return kVendor;
}

std::string
adapter::zigbee::Adapter::GetAdapterName()
{
  return kAdapterName;
}

std::string
adapter::zigbee::Adapter::GetVersion()
{
  return kVersion;
}

std::string
adapter::zigbee::Adapter::GetExposedAdapterPrefix()
{
  return kExposedPrefix;
}

std::string
adapter::zigbee::Adapter::GetExposedApplicationName()
{
  return kExposedAppName;
}

common::Guid
adapter::zigbee::Adapter::GetExposedApplicationGuid()
{
  return kExposedGuid;
}

bridge::AdapterSignalVector
adapter::zigbee::Adapter::GetSignals()
{
  // TODO: wouldn't this list be per device?
  return bridge::AdapterSignalVector();
}

bridge::AdapterStatus
adapter::zigbee::Adapter::Initialize()
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::Shutdown()
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::EnumDevices(
    bridge::EnumDeviceOptions,
    bridge::AdapterDeviceVector&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::GetProperty(
    std::shared_ptr<bridge::IAdapterProperty>&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::SetProperty(
    std::shared_ptr<bridge::IAdapterProperty> const&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::GetPropertyValue(
    std::shared_ptr<bridge::IAdapterProperty> const&,
    std::string const&,
    std::shared_ptr<bridge::IAdapterValue>&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::SetPropertyValue(
    std::shared_ptr<bridge::IAdapterProperty> const&,
    std::shared_ptr<bridge::IAdapterValue> const&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::CallMethod(
    std::shared_ptr<bridge::IAdapterMethod>&,
    std::shared_ptr<bridge::IAdapterIoRequest>*)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::RegisterSignalListener(
  std::string const&,
  std::shared_ptr<bridge::IAdapterSignalListener> const&,
  void*,
  bridge::IAdapter::RegistrationHandle&)
{
  return -1;
}

bridge::AdapterStatus
adapter::zigbee::Adapter::UnregisterSignalListener(bridge::IAdapter::RegistrationHandle const&)
{
  return -1;
}
