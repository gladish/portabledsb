#pragma once

#include "Bridge/IAdapter.h"

namespace adapter
{
namespace zigbee
{
  class Adapter;
  class Device;
  class Value;

  class Method : public bridge::IAdapterMethod
  {
  public:
    Method(weak_ptr<Device> const& target, std::string const& name);

  public:
    virtual std::string GetName() const;
    virtual std::string GetDescritpion() const;

    virtual bridge::AdapterValueVector const& GetInputParams() const;
    virtual bridge::AdapterValueVector const& GetOutputParams() const;

    virtual void SetInputParams(bridge::AdapterValueVector const& params);
    virtual void SetOutputParams(bridge::AdapterValueVector const& params);

    void AddInputParam(shared_ptr<bridge::IAdapterValue> const& p);
    void AddOutputParam(shared_ptr<bridge::IAdapterValue> const& p);

    virtual int32_t GetResult();
    void SetResult(int32_t st);

  private:
    shared_ptr<Device> GetParent() const
      { return m_parent.lock(); }

  private:
    std::string                   m_name;
    std::string                   m_description;
    bridge::AdapterValueVector    m_inputParams;
    bridge::AdapterValueVector    m_outputParams;
    weak_ptr<Device>              m_parent;
    int32_t                       m_result;
  };

  class Adapter : public bridge::IAdapter
  {
  public:
    Adapter();
    virtual ~Adapter();

    virtual std::string GetVendor();
    virtual std::string GetAdapterName();
    virtual std::string GetVersion();
    virtual std::string GetExposedAdapterPrefix();
    virtual std::string GetExposedApplicationName();
    virtual common::Guid GetExposedApplicationGuid();
    virtual bridge::AdapterSignalVector GetSignals();

    virtual int32_t Initialize();
    virtual int32_t Shutdown();

    virtual int32_t EnumDevices(
      bridge::EnumDeviceOptions opts,
      bridge::AdapterDeviceVector& deviceList,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t GetProperty(
      shared_ptr<bridge::IAdapterProperty>& prop,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t SetProperty(
      shared_ptr<bridge::IAdapterProperty> const& prop,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t GetPropertyValue(
      shared_ptr<bridge::IAdapterProperty> const& prop,
      std::string const& attributeName,
      shared_ptr<bridge::IAdapterValue>& value,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t SetPropertyValue(
      shared_ptr<bridge::IAdapterProperty> const& prop,
      shared_ptr<bridge::IAdapterValue> const& value,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t CallMethod(
      shared_ptr<bridge::IAdapterMethod>& method,
      shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual int32_t RegisterSignalListener(
      std::string const& signalName,
      shared_ptr<bridge::IAdapterSignalListener> const& listener,
      void* argp,
      bridge::IAdapter::RegistrationHandle& handle);

    virtual int32_t UnregisterSignalListener(
      bridge::IAdapter::RegistrationHandle const& h);
  };
}
}
