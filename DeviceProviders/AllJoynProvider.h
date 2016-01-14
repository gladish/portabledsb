#pragma once

#include "DeviceProviders/AllJoynStatus.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <alljoyn/AboutListener.h>
#pragma GCC diagnostic pop

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <queue>

class AllJoynService;

class AllJoynProvider : std::enable_shared_from_this<AllJoynProvider>
{
  friend class AboutListener;

public:
  AllJoynProvider(std::string const& appname);
  virtual ~AllJoynProvider();

  AllJoynStatus Start();
  void Shutdown();

  inline ajn::BusAttachment* GetBusAttachment() const
    { return m_bus; }

  void RemoveSession(std::shared_ptr<AllJoynService> const& service);

private:
  void AnnounceDiscovery(char const* serviceName, uint16_t version, ajn::SessionPort port,
    ajn::MsgArg const& objectDescriptionArg, ajn::MsgArg const& aboutDataArg);

  class AboutListener : public ajn::AboutListener
  {
  public:
    AboutListener(AllJoynProvider* provider)
      : m_parent(provider) { }

    virtual ~AboutListener() { }

    virtual void Announced(char const* busName, uint16_t version, ajn::SessionPort port,
        ajn::MsgArg const& objectDescriptionArg, ajn::MsgArg const& aboutDataArg);
  private:
    AllJoynProvider* m_parent;
  };

  void ProcessAboutAnnouncementQueue();

  static void* RunAboutHandlerProcessing(void* argp)
    { ((AllJoynProvider *) argp)->ProcessAboutAnnouncementQueue(); return 0;}

private:
  ajn::BusAttachment*   m_bus;
  bool                  m_alljoynInitialized;
  bool                  m_isRegistered;
  bool                  m_isListening;
  std::string           m_applicationName;
  AboutListener*        m_aboutListener;

  struct WorkItem
  {
    ajn::MsgArg ObjectDescriptionArg;
    ajn::MsgArg AboutDataArg;
    ajn::SessionPort Port;
    std::string ServiceName;

    void Reset()
    {
      ObjectDescriptionArg.Clear();
      AboutDataArg.Clear();
      Port = 0;
      ServiceName.clear();
    }
  };

  bool                  m_aboutQueueRun;
  pthread_t             m_thread;
  pthread_mutex_t       m_mutex;
  pthread_cond_t        m_cond;
  std::queue<WorkItem>  m_aboutHandlerQueue;

  typedef std::vector< std::shared_ptr<AllJoynService> > service_vector_type;
  typedef std::map< std::string, std::shared_ptr<AllJoynService> > service_map_type;

  service_map_type m_servicesMap;
  service_vector_type m_servicesVector;
};

