#include "AllJoynProvider.h"
#include "AllJoynService.h"

#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>

#include <algorithm>

namespace
{
  template<class T>
  class IsSamePointer
  {
  public:
    IsSamePointer(T* p) : m_ptr(p) { }
    bool operator()(std::shared_ptr<T> const& rhs)
      { return m_ptr == rhs.get(); }
  private:
    T* m_ptr; 
  };
}

void AllJoynProvider::AboutListener::Announced(
  char const*         serviceName,
  uint16_t            version,
  ajn::SessionPort    port,
  ajn::MsgArg const&  objectDescriptionArg,
  ajn::MsgArg const&  aboutDataArg)
{
  m_parent->AnnounceDiscovery(serviceName, version, port, objectDescriptionArg,
    aboutDataArg);
}


AllJoynProvider::AllJoynProvider(std::string const& appname)
  : m_bus(NULL)
  , m_alljoynInitialized(false)
  , m_isRegistered(false)
  , m_isListening(false)
  , m_applicationName(appname)
  , m_aboutListener(NULL)
  , m_aboutQueueRun(false)
{
  pthread_mutex_init(&m_mutex, NULL);
  pthread_cond_init(&m_cond, NULL);
}

AllJoynProvider::~AllJoynProvider()
{
  Shutdown();
}

AllJoynStatus AllJoynProvider::Start()
{
  QStatus status = ER_OK;

  #define AJ_CHECK(ST) { status = ST; if (status != ER_OK) return AllJoynStatus(status); }

  if (m_bus != NULL)
    return AllJoynStatus(ER_FAIL);

  if (!m_alljoynInitialized)
  {
    AJ_CHECK( AllJoynInit() );
    m_alljoynInitialized = true;
  }

  m_bus = new ajn::BusAttachment(m_applicationName.c_str(), true);
  AJ_CHECK( m_bus->Start() );
  AJ_CHECK( m_bus->Connect() );

  pthread_create(&m_thread, NULL, RunAboutHandlerProcessing, this);

  m_aboutListener = new AboutListener(this);
  m_bus->RegisterAboutListener(*m_aboutListener);
  m_isRegistered = true;

  AJ_CHECK( m_bus->WhoImplements(NULL) );

  m_isListening = true;

  return AllJoynStatus(status);
}

void
AllJoynProvider::Shutdown()
{
  pthread_mutex_lock(&m_mutex);
  m_aboutQueueRun = false;
  pthread_mutex_unlock(&m_mutex);
  pthread_cond_signal(&m_cond);
  pthread_join(m_thread, NULL);

  if (m_isListening)
  {
    m_bus->CancelWhoImplements(NULL);
    m_isListening = false;
  }

  if (m_aboutListener)
  {
    if (m_isRegistered)
    {
      m_bus->UnregisterAboutListener(*m_aboutListener);
      m_isRegistered = false;
    }
    delete m_aboutListener;
    m_aboutListener = NULL;
  }

  m_servicesVector.clear();
  m_servicesMap.clear();

  if (m_bus)
  {
    m_bus->Stop();
    m_bus->Join();
    delete m_bus;
    m_bus = NULL;
  }

  if (m_alljoynInitialized)
  {
    AllJoynShutdown();
    m_alljoynInitialized = false;
  }
}

void
AllJoynProvider::ProcessAboutAnnouncementQueue()
{
  typedef std::map<std::string, std::shared_ptr<AllJoynService> > service_map_type;

  WorkItem item;

  while (true)
  {
    item.Reset();

    pthread_mutex_lock(&m_mutex);
    while (m_aboutQueueRun && m_aboutHandlerQueue.empty())
      pthread_cond_wait(&m_cond, &m_mutex);
    if (m_aboutQueueRun)
    {
      item = m_aboutHandlerQueue.front();
      m_aboutHandlerQueue.pop();
    }
    pthread_mutex_unlock(&m_mutex);

    if (!m_aboutQueueRun)
      return;

    pthread_mutex_lock(&m_mutex);
    service_map_type::iterator itr = m_servicesMap.find(item.ServiceName);
    if (itr != m_servicesMap.end())
    {
      itr->second->Initialize(item.AboutDataArg, item.ObjectDescriptionArg);
    }
    else
    {
      std::shared_ptr<AllJoynService> service(AllJoynService::Create(shared_from_this(), item.ServiceName, item.Port));
      m_servicesMap[item.ServiceName] = service;
      m_servicesVector.push_back(service);
    }
    pthread_mutex_unlock(&m_mutex);
  }
}

void
AllJoynProvider::RemoveSession(std::shared_ptr<AllJoynService> const& service)
{
  // TODO: Does this need to be on another thread? 
  service_vector_type::iterator itr;

  pthread_mutex_lock(&m_mutex);

  itr = std::find_if(m_servicesVector.begin(), m_servicesVector.end(), IsSamePointer<AllJoynService>(service.get()));
  if (itr != m_servicesVector.end())
    m_servicesVector.erase(itr);

  m_servicesMap.erase(service->GetName());
  pthread_mutex_unlock(&m_mutex);

  service->Shutdown();
}


void
AllJoynProvider::AnnounceDiscovery(char const* serviceName, uint16_t /*version*/, ajn::SessionPort port,
      ajn::MsgArg const& objectDescriptionArg, ajn::MsgArg const& aboutDataArg)
{
  pthread_mutex_lock(&m_mutex);

  WorkItem item;
  item.ObjectDescriptionArg = objectDescriptionArg;
  item.AboutDataArg = aboutDataArg;
  item.ServiceName = serviceName;
  item.Port = port;
  m_aboutHandlerQueue.push(item);

  pthread_mutex_unlock(&m_mutex);
  pthread_cond_signal(&m_cond);
}

