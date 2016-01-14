#pragma once

#include "Adapters/MockAdapter/MockDevices.h"
#include "Adapters/MockAdapter/MockAdapterMethod.h"
#include "Adapters/MockAdapter/MockAdapterProperty.h"
#include "Adapters/MockAdapter/MockAdapterSignal.h"
#include "Adapters/MockAdapter/MockAdapterValue.h"

#include "Bridge/IAdapter.h"

#include <memory>

namespace adapters
{
namespace mock
{
  std::string const kDeviceResetMethod = "Device_Reset";
  std::string const kDeviceResetPropertyHandle = "Property_Handle";
  std::string const kDsbMethodReturnValue = "Return_Value";

  class MockAdapter;
  class MockAdapterDevice;
  class MockAdapterValue;

  class MockAdapterDevice : public bridge::IAdapterDevice, public std::enable_shared_from_this<MockAdapterDevice> 
  {
  public:
    static std::shared_ptr<MockAdapterDevice> Create(
      MockDeviceDescriptor const& desc,
      std::weak_ptr<MockAdapter> const& parent);

    virtual std::string GetName();
    virtual std::string GetVendor();
    virtual std::string GetModel();
    virtual std::string GetVersion();
    virtual std::string GetFirmwareVersion();
    virtual std::string GetSerialNumber();
    virtual std::string GetDescription();
    virtual std::shared_ptr<bridge::IAdapterIcon> GetIcon();

    virtual bridge::AdapterPropertyVector const& GetProperties() const;
    virtual bridge::AdapterMethodVector const& GetMethods() const;
    virtual bridge::AdapterSignalVector const& GetSignals() const;

    int32_t DispatchMethod(
      std::shared_ptr<MockAdapterMethod>& method,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    int32_t SendSignal(
      std::string const& signalName,
      std::shared_ptr<MockAdapterProperty> const& prop,
      std::shared_ptr<MockAdapterValue> const& attr);

    std::shared_ptr<MockAdapter> GetParent() const
      { return m_parent.lock(); }

  private:
    MockAdapterDevice(MockDeviceDescriptor const& desc, std::weak_ptr<MockAdapter> const& parent);

    void CreateMethods();
    void CreateSignals();

    int32_t Reset(std::shared_ptr<MockAdapterMethod>& method);

  private:
    std::string                         m_name;
    std::weak_ptr<MockAdapter>               m_parent;
    std::string                         m_vendor;
    std::string                         m_model;
    std::string                         m_version;
    std::string                         m_firmwareVersion;
    std::string                         m_serialNumber;
    std::string                         m_description;
    std::shared_ptr<bridge::IAdapterIcon>    m_icon;
    bridge::AdapterPropertyVector       m_properties;
    bridge::AdapterMethodVector         m_methods; 
    bridge::AdapterSignalVector         m_signalPrototypes;
  };
}
}

