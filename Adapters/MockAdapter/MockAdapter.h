#pragma once

#include "Bridge/IAdapter.h"
#include "Common/defines.h"

#include <map>

namespace adapters
{
namespace mock
{
  class MockAdapterDevice;
  class MockAdapterSignal;

  class MockAdapter : public bridge::IAdapter, public enable_shared_from_this<bridge::IAdapter>
  {
  public:
    MockAdapter();
    virtual ~MockAdapter();

    virtual std::string GetVendor();
    virtual std::string GetAdapterName();
    virtual std::string GetVersion();
    virtual std::string GetExposedAdapterPrefix();
    virtual std::string GetExposedApplicationName();
    virtual common::Guid GetExposedApplicationGuid();
    virtual bridge::AdapterSignalVector GetSignals();

    virtual int32_t Initialize();
    virtual int32_t Shutdown();

    virtual int32_t GetConfiguration(std::vector<uint8_t>* /*configData*/)
      { return 0; }

    virtual int32_t SetConfiguration(std::vector<uint8_t> const& /*configData*/)
      { return 0; }

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

    virtual int32_t UnregisterSignalListener(bridge::IAdapter::RegistrationHandle const& h);

    int32_t NotifySignalListeners(shared_ptr<MockAdapterSignal> const& signal);

  private:
    void CreateMockDevices();
    void CreateSignals();

  private:
    std::string m_vendor;
    std::string m_adapterName;
    std::string m_version;
    std::string m_exposedAdapterPrefix;
    std::string m_exposedApplicationName;
    common::Guid m_exposedApplicationGuid;

    std::vector< shared_ptr<MockAdapterDevice> > m_devices;
    std::vector< shared_ptr<MockAdapterSignal> > m_signals;

    struct RegisteredSignal
    {
      shared_ptr<bridge::IAdapterSignalListener> Listener;
      void* Context;
      bridge::IAdapter::RegistrationHandle RegHandle;
    };

    typedef std::multimap<std::string, RegisteredSignal> SignalMap;
    SignalMap m_signalListeners;
  };
}
}

