#pragma once

#include "Bridge/AllJoynHeaders.h"

#include "Common/Adapter.h"
#include "Common/AdapterDevice.h"

#include <memory>


namespace bridge
{
  class BusObject : public ajn::BusObject
  {
  public:
   virtual ~BusObject();
   void Publish();

  private:
    void AnnounceAndRegister();

  private:
    BusObject(std::string const& appname, std::string const& path);

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
    std::unique_ptr<ajn::BusAttachment>   m_bus;
    adapter::Device                       m_adapterDevice;
    std::shared_ptr<adapter::Adapter>     m_adapter;
    std::unique_ptr<SessionPortListener>  m_sessionPortListener;
    ajn::SessionPort                      m_sessionPort;
    std::unique_ptr<ajn::AboutObj>        m_aboutObject;
    std::unique_ptr<ajn::AboutData>       m_aboutData;
    std::unique_ptr<BusListener>          m_busListener;
    std::string                           m_appName;
  };
}
