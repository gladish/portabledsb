#ifndef __DEVICE_MAIN_H__
#define __DEVICE_MAIN_H__

#include "BridgeDevice.h"
#include <map>

#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>

namespace bridge
{
  class DeviceMethod;
  class DeviceSignal;

  class DeviceMain : private ajn::BusObject
  {
    DeviceMain(BridgeDevice& parent);
    virtual ~DeviceMain();

    QStatus Initialize();

    bool IsMethodNameUnique(std::string const& name);
    bool IsSignalNameUnique(std::string const& name);
    void HandleSignal(IAdapterSignal const& adapterSignal);

    inline int GetIndexForMethod()
      { return m_indexForMethod++; }

    inline std::string GetBusObjectPath(IAdapterProperty const&)
      { return std::string(); }

  private:
    void Shutdown();
    QStatus CreateMethodsAndSignals();
    void AJMethod(const ajn::InterfaceDescription::Member*, ajn::Message&);

  private:
    BridgeDevice&                               m_parent;
    int                                         m_indexForSignal;
    int                                         m_indexForMethod;
    std::unique_ptr<ajn::InterfaceDescription>  m_interfaceDescription;
    std::string                                 m_interfaceName;
    std::map<std::string, DeviceMethod* >       m_deviceMethods;
    std::map<std::string, DeviceSignal* >       m_deviceSignals;
    bool                                        m_registeredOnAllJoyn;
  };
}

#endif

