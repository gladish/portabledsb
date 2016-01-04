#pragma once

#include "Bridge/IAdapter.h"
#include <alljoyn/BusAttachment.h>

namespace bridge
{
  class BridgeDevice
  {
  public:
    BridgeDevice(const shared_ptr<IAdapterDevice>&, const shared_ptr<IAdapter>&);
    virtual ~BridgeDevice();

    QStatus Shutdown();
    QStatus Initialize();

    ajn::BusAttachment& GetBusAttachment()
    {
      return m_busAttachment;
    }

    shared_ptr<IAdapterDevice> GetAdapterDevice()
    {
      return m_parent;
    }

  private:
    shared_ptr<IAdapterDevice> m_parent;
    ajn::BusAttachment m_busAttachment;
  };
}

