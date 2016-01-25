#include "Mutex.h"

adapter::Mutex::Mutex()
{
}

void
adapter::Mutex::Lock()
{
  m_mutex.lock();
}

void
adapter::Mutex::Unlock()
{
  m_mutex.unlock();
}

