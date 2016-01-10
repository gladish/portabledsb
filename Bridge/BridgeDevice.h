#pragma once

#include "AllJoynAbout.h"
#include "IAdapter.h"
#include "DeviceMain.h"
#include "DeviceProperty.h"
#include <alljoyn/BusAttachment.h>

namespace bridge
{
  class BridgeDevice : private ajn::SessionPortListener, private ajn::BusListener
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
      return m_device;
    }

    inline std::string &GetRootNameForInterface()
    {
      return m_rootStringForAllJoynNames;
    }

  private:
    QStatus BuildServiceName();
    QStatus InitializeAllJoyn();
    QStatus CreateDeviceProperties();
    QStatus InitControlPanel();
    QStatus InitLightingService();
    QStatus ConnectToAllJoyn();
    QStatus ShutdownAllJoyn();
    QStatus RegisterSignalHandlers(bool isRegister);

    shared_ptr<IAdapterDevice> m_device;
    shared_ptr<IAdapter> m_adapter;
    ajn::BusAttachment m_busAttachment;
    DeviceMain m_deviceMain;
    AllJoynAbout m_about;
    std::string m_rootStringForAllJoynNames;
    std::string m_serviceName;
    std::map<std::string, shared_ptr<DeviceProperty>> m_deviceProperties;
  };
}

