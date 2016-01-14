#include "AllJoynService.h"
#include "AllJoynProvider.h"
#include <alljoyn/Session.h>
#include <pthread.h>

AllJoynService::AllJoynService(std::shared_ptr<AllJoynProvider> const& provider, std::string const& serviceName, ajn::SessionPort port)
  : m_provider(provider)
  , m_name(serviceName)
  , m_sessionPort(port)
  , m_sessionListener(NULL)
{
  pthread_mutex_init(&m_mutex, NULL);
  m_sessionListener = new SessionListener(this);

  ajn::SessionOpts sessionOpts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY,
    ajn::TRANSPORT_ANY);

  ajn::BusAttachment* bus = GetProvider()->GetBusAttachment();
  if (bus)
  {
    bus->JoinSession(m_name.c_str(), m_sessionPort, m_sessionListener, m_sessionId,
        sessionOpts);
  }
}

AllJoynService::~AllJoynService()
{
  Shutdown();
}

void
AllJoynService::Initialize(ajn::MsgArg const& aboutDataArg, ajn::MsgArg const& objectDescriptionArg)
{
  m_aboutDataArg = aboutDataArg;
  m_objectDescriptionArg = objectDescriptionArg;
}

void
AllJoynService::Shutdown()
{
  if (m_sessionId != 0)
  {
    ajn::BusAttachment* bus = GetProvider()->GetBusAttachment();
    if (bus)
      bus->LeaveSession(m_sessionId);
    m_sessionId = 0;
  }

  if (m_sessionListener)
  {
    delete m_sessionListener;
    m_sessionListener = NULL;
  }

  pthread_mutex_lock(&m_mutex);
  m_objectsMap.clear();
  pthread_mutex_unlock(&m_mutex);
}

std::shared_ptr<AllJoynBusObject>
AllJoynService::GetBusObject(std::string const& path)
{
  std::shared_ptr<AllJoynBusObject> busObject;

  pthread_mutex_lock(&m_mutex);
  object_map_type::iterator itr = m_objectsMap.find(path);
  if (itr != m_objectsMap.end())
  {
    busObject = itr->second.lock();
    if (busObject)
      m_objectsMap.erase(itr);
  }
  pthread_mutex_unlock(&m_mutex);
  return busObject;
}

ajn::BusAttachment*
AllJoynService::GetBusAttachment() const
{
  return GetProvider()->GetBusAttachment();
}

void
AllJoynService::OnSessionLost(ajn::SessionId /*sessionId*/, ajn::SessionListener::SessionLostReason /*reason*/)
{
  std::shared_ptr<AllJoynProvider> provider = GetProvider();
  if (provider)
  {
    provider->RemoveSession(shared_from_this());
  }
}

void
AllJoynService::OnSessionMemberAdded(ajn::SessionId /*sessionId*/, char const* /*uniqueName*/)
{

}

void
AllJoynService::OnSessionMemberRemoved(ajn::SessionId /*sessionId*/, char const* /*uniqueName*/)
{

}

void
AllJoynService::SessionListener::SessionLost(ajn::SessionId sessionId, ajn::SessionListener::SessionLostReason reason)
{
  m_parent->OnSessionLost(sessionId, reason);
}

void
AllJoynService::SessionListener::SessionMemberAdded(ajn::SessionId sessionId, char const* uniqueName)
{
  m_parent->OnSessionMemberAdded(sessionId, uniqueName);
}

void
AllJoynService::SessionListener::SessionMemberRemoved(ajn::SessionId sessionId, char const* uniqueName)
{
  m_parent->OnSessionMemberRemoved(sessionId, uniqueName);
}
