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

#include "AdapterDevice.h"
#include "AdapterIoRequest.h"
#include "AdapterItemInformation.h"
#include "AdapterLog.h"
#include "AdapterSignal.h"
#include "AdapterSignalListener.h"
#include "AdapterStatus.h"
#include "EnumDeviceOptions.h"
#include "Guid.h"

#include <memory>

namespace adapter
{
  typedef uint32_t RegistrationHandle;

  class Adapter
  {
  protected:
    Adapter() { }

  public:
    virtual ~Adapter() { }

    virtual std::string
    GetAdapterPrefix() = 0;

    virtual std::string
    GetApplicationName() = 0;

    virtual Guid
    GetApplicationGuid() = 0;

    virtual std::string
    GetStatusText(adapter::Status st) = 0;

    virtual ItemInformation const&
    GetInfo() = 0;

    virtual adapter::Status SetConfiguration(
      std::vector<uint8_t> const& configData,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status GetConfiguration(
      std::vector<uint8_t>& configData,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status Initialize(
      std::shared_ptr<adapter::Log> const& log,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status Shutdown(
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status GetSignals(
      Signal::Vector& signals,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status EnumDevices(
      EnumDeviceOptions opts,
      Device::Vector& devices,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status GetProperty(
      Device const& device,
      Interface const& ifc,
      Property const& prop,
      Value& value,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status SetProperty(
      Device const& device,
      Interface const& ifc,
      Property const& prop,
      Value const& value,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status InvokeMethod(
      Device const& device,
      Interface const& ifc,
      Method const& method,
      Value const& inarg,
      Value& outarg,
      IoRequest::Pointer const& req) = 0;

    virtual adapter::Status RegisterSignalListener(
      Device const& device,
      Interface const& ifc,
      Signal const& signal,
      SignalListener const& listener,
      void* argp,
      adapter::RegistrationHandle& handle) = 0;

    virtual adapter::Status UnregisterSignalListener(
      RegistrationHandle const& h) = 0;
  };
}
