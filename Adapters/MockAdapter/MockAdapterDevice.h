#pragma once

#include "Adapters/MockAdapter/MockDevices.h"
#include "Bridge/IAdapter.h"

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

  class MockAdapterSignal : public bridge::IAdapterSignal
  {
  public:
    MockAdapterSignal(
      std::string const& name,
      weak_ptr<MockAdapterDevice> const& parent,
      bridge::AdapterValueVector const& params);

    virtual std::string GetName() const;
    virtual bridge::AdapterValueVector const& GetParams() const;

    shared_ptr<MockAdapterSignal> Clone();

  private:
    std::string                     m_name;
    weak_ptr<MockAdapterDevice>     m_parent;
    bridge::AdapterValueVector      m_params;
  };

  class MockAdapterMethod : public bridge::IAdapterMethod
  {
    friend class MockAdapter;

  public:
    MockAdapterMethod(std::string const& name, weak_ptr<MockAdapterDevice> const& parent);

    virtual std::string GetName() const;
    virtual std::string GetDescription();

    virtual bridge::AdapterValueVector const& GetInputParams() const;
    virtual bridge::AdapterValueVector const& GetOutputParams() const;

    virtual void SetInputParams(bridge::AdapterValueVector const& params);
    virtual void SetOutputParams(bridge::AdapterValueVector const& params);

    void AddInputParam(shared_ptr<bridge::IAdapterValue> const& p);
    void AddOutputParam(shared_ptr<bridge::IAdapterValue> const& p);

    virtual int32_t GetResult();
    void SetResult(int32_t st);

  private:
    shared_ptr<MockAdapterDevice> GetParent() const
      { return m_parent.lock(); }

  private:
    std::string                   m_name;
    std::string                   m_description;
    bridge::AdapterValueVector    m_inputParams;
    bridge::AdapterValueVector    m_outputParams;
    weak_ptr<MockAdapterDevice>   m_parent;
    int32_t                       m_result;
  };

  class MockAdapterProperty : public bridge::IAdapterProperty
  {
  public:
    MockAdapterProperty(MockPropertyDescriptor const& desc, weak_ptr<MockAdapterDevice> const& parent);

    virtual std::string GetName();
    virtual std::string GetInterfaceHint()
      { return std::string(""); }

    virtual bridge::AdapterValueVector GetAttributes();

    shared_ptr<MockAdapterDevice> GetParent()
      { return m_parent.lock(); }

    shared_ptr<MockAdapterValue> GetAttributeByName(std::string const& name);

  private:
    std::string                 m_name;
    bridge::AdapterValueVector  m_attributes;
    weak_ptr<MockAdapterDevice> m_parent;
  };

  class MockAdapterValue : public bridge::IAdapterValue
  {
  public:
    MockAdapterValue(std::string const& name);

    virtual std::string GetName();
    virtual common::Variant const& GetData() const;
    virtual void SetData(common::Variant const& v);

  private:
    std::string                 m_name;
    common::Variant             m_data;
  };

  class MockAdapterDevice : public bridge::IAdapterDevice, public enable_shared_from_this<MockAdapterDevice> 
  {
  public:
    static shared_ptr<MockAdapterDevice> Create(
      MockDeviceDescriptor const& desc,
      weak_ptr<MockAdapter> const& parent);

    virtual std::string GetName();
    virtual std::string GetVendor();
    virtual std::string GetModel();
    virtual std::string GetVersion();
    virtual std::string GetFirmwareVersion();
    virtual std::string GetSerialNumber();
    virtual std::string GetDescription();
    virtual shared_ptr<bridge::IAdapterIcon> GetIcon();

    virtual bridge::AdapterPropertyVector const& GetProperties() const;
    virtual bridge::AdapterMethodVector const& GetMethods() const;
    virtual bridge::AdapterSignalVector const& GetSignals() const;

    int32_t DispatchMethod(
      shared_ptr<MockAdapterMethod>& method,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    int32_t SendSignal(
      std::string const& signalName,
      shared_ptr<MockAdapterProperty> const& prop,
      shared_ptr<MockAdapterValue> const& attr);

    shared_ptr<MockAdapter> GetParent() const
      { return m_parent.lock(); }

  private:
    MockAdapterDevice(MockDeviceDescriptor const& desc, weak_ptr<MockAdapter> const& parent);

    void CreateMethods();
    void CreateSignals();

    int32_t Reset(shared_ptr<MockAdapterMethod>& method);

  private:
    std::string                         m_name;
    weak_ptr<MockAdapter>               m_parent;
    std::string                         m_vendor;
    std::string                         m_model;
    std::string                         m_version;
    std::string                         m_firmwareVersion;
    std::string                         m_serialNumber;
    std::string                         m_description;
    shared_ptr<bridge::IAdapterIcon>    m_icon;
    bridge::AdapterPropertyVector       m_properties;
    bridge::AdapterMethodVector         m_methods; 
    bridge::AdapterSignalVector         m_signalPrototypes;
  };
}
}

