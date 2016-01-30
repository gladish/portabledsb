#pragma once

#include "Bridge/ConfigManager.h"
#include "Common/Adapter.h"

#include <alljoyn/Status.h>

#include <map>
#include <string>

namespace bridge
{
  class BusObject;

  class Bridge : public std::enable_shared_from_this<Bridge>
  {
  public:
    static std::shared_ptr<Bridge> GetInstance();
    static void InitializeSingleton(std::shared_ptr<adapter::Adapter> const& adapter);
    static void ReleaseSingleton();

    virtual ~Bridge();

    void Initialize();
    void Shutdown();

  private:
    Bridge(std::shared_ptr<adapter::Adapter> const& adapter);

    QStatus RegisterAdapterSignalHandlers(bool isRegister);
    void InitializeAdapter();
    void InitializeInternal();
    void InitializeDevices(bool isUpdate = false);
    void CreateDevice(std::shared_ptr<adapter::Adapter> const& adapter, adapter::Device const& device);
    void UpdateDevice(adapter::Device const& device, bool exposedOnAllJoynBus);
    void OnAdapterSignal(adapter::Signal const& signal, void* argp);

  private:
    bool                                      m_alljoynInitialized;
    std::shared_ptr<adapter::Adapter>         m_adapter;
    std::vector<adapter::RegistrationHandle>  m_registeredSignalListeners;
    std::vector< std::shared_ptr<BusObject> > m_bridgeBusObjects;
    ConfigManager                             m_configManager;
  };
}

