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

#include "bridge/BridgeConfig.h"
#include "bridge/AllJoynHeaders.h"
#include "common/Adapter.h"

#include <memory>

namespace bridge
{
  class Bridge;

  class ConfigManager : private ajn::BusListener, private ajn::SessionListener, private ajn::SessionPortListener
  {
  public:
    ConfigManager(Bridge&, std::shared_ptr<adapter::Adapter> const& adapter);
    ~ConfigManager();

    QStatus Initialize(char const* configFile);
    QStatus ConnectToAllJoyn();
    QStatus Shutdown();

    bool IsConfigurationAccessSecured();
    bool IsDeviceAccessSecured();
    bool GetObjectConfigItem();
    void ToFile();

    std::shared_ptr<BridgeConfig> GetBridgeConfig();

  private:
    QStatus ShutdownAllJoyn();
    QStatus InitializeCSPBusObjects();
    QStatus BuildServiceName();

    /* ajn::SessionPortListener */
    virtual bool AcceptSessionJoiner(ajn::SessionPort, const char *joiner, const ajn::SessionOpts&);
    virtual void SessionJoined(ajn::SessionPort, ajn::SessionId, const char *joiner);

    /* ajn::SessionListener */
    virtual void SessionMemberRemoved(ajn::SessionId, const char *uniqueName);
    virtual void BusDisconnected();

  private:
    Bridge& m_parent;
    std::shared_ptr<adapter::Adapter>   m_adapter;
    std::unique_ptr<ajn::BusAttachment> m_bus;
    std::string m_serviceName;

    // BridgeAuthHandler
    BridgeConfig m_bridgeConfig;
    ajn::SessionPort m_sessionPort;
  };
};

