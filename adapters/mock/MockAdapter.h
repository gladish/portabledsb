//
// Copyright (c) 2016, Comcast Cable Communications Management, LLC
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or
// without fee is hereby granted, provided that the above copyright notice and this
// permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
// SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
// ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.
//
#pragma once

#include "common/Adapter.h"

#include <map>
#include <memory>

namespace adapters
{
namespace mock
{
  std::shared_ptr<adapter::Interface> GetZigBeeCluster(uint16_t clusterId);

  class MockAdapter : public adapter::Adapter, public std::enable_shared_from_this<adapter::Adapter>
  {
  public:
    MockAdapter();
    virtual ~MockAdapter();

    virtual adapter::ItemInformation const&
    GetInfo();

    virtual std::string GetAdapterPrefix();
    virtual std::string GetApplicationName();
    virtual adapter::Guid GetApplicationGuid();

    virtual adapter::Status GetSignals(
        adapter::Signal::Vector& signals,
        std::shared_ptr<adapter::IoRequest> const& req);

    virtual std::string GetStatusText(adapter::Status st);

    virtual adapter::Status Initialize(
        std::shared_ptr<adapter::Log> const& log,
        std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status Shutdown(
        std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status GetConfiguration(
        std::vector<uint8_t>& configData,
        std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status SetConfiguration(
        std::vector<uint8_t> const& configData,
        std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status EnumDevices(
      adapter::EnumDeviceOptions opts,
      adapter::Device::Vector& devices,
      std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status GetProperty(
      adapter::Device const& device,
      adapter::Interface const& ifc,
      adapter::Property const& prop,
      adapter::Value& value,
      std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status SetProperty(
      adapter::Device const& device,
      adapter::Interface const& ifc,
      adapter::Property const& prop, 
      adapter::Value const& value,
      std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status InvokeMethod(
      adapter::Device const& device,
      adapter::Interface const& ifc,
      adapter::Method const& method,
      adapter::Value const& inarg,
      adapter::Value& outarg,
      std::shared_ptr<adapter::IoRequest> const& req);

    virtual adapter::Status RegisterSignalListener(
      adapter::Device const& device,
      adapter::Interface const& ifc,
      adapter::Signal const& signal,
      adapter::SignalListener const& listener,
      void* argp,
      adapter::RegistrationHandle& handle);

    virtual adapter::Status UnregisterSignalListener(adapter::RegistrationHandle const& h);

    adapter::Status NotifySignalListeners(adapter::Signal const& signal);

  private:
    void CreateMockDevices();

    adapter::Device CreateDoorWindowSensor(std::string const& name);
    adapter::Device CreateMultiEndpointPowerStrip(std::string const& name);

  private:
    adapter::ItemInformation m_info;

    std::string m_exposedAdapterPrefix;
    std::string m_exposedApplicationName;
    adapter::Guid m_exposedApplicationGuid;
    std::shared_ptr<adapter::Log> m_log;

    adapter::Device::Vector m_devices;
    adapter::Signal::Vector m_signals;

    struct RegisteredSignal
    {
      adapter::SignalListener Listener;
      void* Context;
      adapter::RegistrationHandle RegHandle;
    };

    typedef std::multimap<std::string, RegisteredSignal> SignalMap;
    SignalMap m_signalListeners;
  };
}
}

