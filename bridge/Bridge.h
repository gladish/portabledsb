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

#include "bridge/ConfigManager.h"
#include "common/Adapter.h"

#include <alljoyn/Status.h>

#include <map>
#include <string>

namespace bridge
{
  class BusObject;

  class Bridge : public std::enable_shared_from_this<Bridge>
  {
  public:
    static std::shared_ptr<Bridge> GetInstance();
    static void InitializeSingleton(std::shared_ptr<adapter::Adapter> const& adapter);
    static void ReleaseSingleton();

    virtual ~Bridge();

    void Initialize(char const* configFile);
    void Shutdown();

  private:
    Bridge(std::shared_ptr<adapter::Adapter> const& adapter);

    QStatus RegisterAdapterSignalHandlers(bool isRegister);
    void InitializeAdapter();
    void InitializeInternal(char const* configFile);
    void InitializeDevices(bool isUpdate = false);
    void CreateDevice(std::shared_ptr<adapter::Adapter> const& adapter, adapter::Device const& device);
    void UpdateDevice(adapter::Device const& device, bool exposedOnAllJoynBus);
    void OnAdapterSignal(adapter::Signal const& signal, void* argp);

  private:
    bool                                      m_alljoynInitialized;
    std::shared_ptr<adapter::Adapter>         m_adapter;
    std::vector<adapter::RegistrationHandle>  m_registeredSignalListeners;
    std::vector< std::shared_ptr<BusObject> > m_bridgeBusObjects;
    ConfigManager                             m_configManager;
  };
}

