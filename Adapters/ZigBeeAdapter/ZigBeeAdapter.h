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
    Method(std::weak_ptr<Device> const& target, std::string const& name);

  public:
    virtual std::string GetName() const;
    virtual std::string GetDescritpion() const;

    virtual bridge::AdapterValueVector const& GetInputParams() const;
    virtual bridge::AdapterValueVector const& GetOutputParams() const;

    virtual void SetInputParams(bridge::AdapterValueVector const& params);
    virtual void SetOutputParams(bridge::AdapterValueVector const& params);

    void AddInputParam(std::shared_ptr<bridge::IAdapterValue> const& p);
    void AddOutputParam(std::shared_ptr<bridge::IAdapterValue> const& p);

    virtual int32_t GetResult();
    void SetResult(int32_t st);

  private:
    std::shared_ptr<Device> GetParent() const
      { return m_parent.lock(); }

  private:
    std::string                   m_name;
    std::string                   m_description;
    bridge::AdapterValueVector    m_inputParams;
    bridge::AdapterValueVector    m_outputParams;
    std::weak_ptr<Device>              m_parent;
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

    virtual bridge::AdapterStatus Initialize();
    virtual bridge::AdapterStatus Shutdown();

    virtual bridge::AdapterStatus EnumDevices(
      bridge::EnumDeviceOptions opts,
      bridge::AdapterDeviceVector& deviceList,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus GetProperty(
      std::shared_ptr<bridge::IAdapterProperty>& prop,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus SetProperty(
      std::shared_ptr<bridge::IAdapterProperty> const& prop,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus GetPropertyValue(
      std::shared_ptr<bridge::IAdapterProperty> const& prop,
      std::string const& attributeName,
      std::shared_ptr<bridge::IAdapterValue>& value,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus SetPropertyValue(
      std::shared_ptr<bridge::IAdapterProperty> const& prop,
      std::shared_ptr<bridge::IAdapterValue> const& value,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus CallMethod(
      std::shared_ptr<bridge::IAdapterMethod>& method,
      std::shared_ptr<bridge::IAdapterIoRequest>* req);

    virtual bridge::AdapterStatus RegisterSignalListener(
      std::string const& signalName,
      std::shared_ptr<bridge::IAdapterSignalListener> const& listener,
      void* argp,
      bridge::IAdapter::RegistrationHandle& handle);

    virtual bridge::AdapterStatus UnregisterSignalListener(
      bridge::IAdapter::RegistrationHandle const& h);
  };
}
}
