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

#include "bridge/AllJoynHeaders.h"
#include "common/Adapter.h"
#include "common/AdapterDevice.h"

#include <memory>


namespace bridge
{
  class BusObject : public ajn::BusObject
  {
  public:
    virtual ~BusObject();
    void Publish();

  private:
    void PublishInternal(bool isChild);
    void AnnounceAndRegister();

  private:
    BusObject(std::string const& appname, std::string const& path);

    BusObject(std::string const& appname, std::string const& path,
      std::shared_ptr<ajn::BusAttachment> const& bus);

    class SessionPortListener : public ajn::SessionPortListener
    {
    public:
      SessionPortListener(BusObject& parent) : m_parent(parent) { }
      virtual ~SessionPortListener() { }
      virtual bool AcceptSessionJoiner(ajn::SessionPort sessionPort, char const* jointer,
        ajn::SessionOpts const& opts);
      virtual void SessionJoined(ajn::SessionPort sessionPort, ajn::SessionId id, char const* joiner);
    private:
      BusObject& m_parent;
    };

    class SessionListener : public ajn::SessionListener
    {
    public:
      SessionListener(BusObject& parent) : m_parent(parent) { }
      virtual ~SessionListener() { }
      virtual void  SessionLost(ajn::SessionId sessionId, SessionLostReason reason);
      virtual void  SessionMemberAdded(ajn::SessionId sessionId, char const* uniqueName);
      virtual void  SessionMemberRemoved(ajn::SessionId sessionId, char const* uniqueName);
    private:
      BusObject& m_parent;
    };

    class BusListener : public ajn::BusListener
    {
    public:
      BusListener(BusObject& parent) : m_parent(parent) { }
      virtual void BusDisconnected();
    private:
      BusObject& m_parent;
    };

  protected:
    virtual QStatus Get(char const* ifcName, char const* propName, ajn::MsgArg &val);
    virtual QStatus Set(char const* ifcName, char const* propName, ajn::MsgArg &val);

  public:
  	static std::shared_ptr<BusObject> BuildFromAdapterDevice(std::string const& appname,
      std::string const& path, std::shared_ptr<adapter::Adapter> const& adapter,
      adapter::Device const& dev);

  private:
    ajn::InterfaceDescription const* BuildInterface(adapter::Interface const& interface);
    void AddPropertiesToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc);
    void AddMethodsToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc);
    void AddSignalsToInterface(adapter::Interface const& interface, ajn::InterfaceDescription& desc);

    void RegisterMethodHandlers();
    void MethodHandler(ajn::InterfaceDescription::Member const* member,
      ajn::Message& message);

  private:
    std::shared_ptr<ajn::BusAttachment>   m_bus;
    adapter::Device                       m_adapterDevice;
    std::shared_ptr<adapter::Adapter>     m_adapter;
    std::unique_ptr<SessionPortListener>  m_sessionPortListener;
    ajn::SessionPort                      m_sessionPort;
    std::unique_ptr<ajn::AboutObj>        m_aboutObject;
    std::unique_ptr<ajn::AboutData>       m_aboutData;
    std::unique_ptr<BusListener>          m_busListener;
    std::string                           m_appName;
    std::vector< std::shared_ptr<BusObject> > m_children;
  };
}
