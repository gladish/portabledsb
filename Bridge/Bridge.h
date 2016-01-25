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

    inline std::shared_ptr<adapter::Adapter> GetAdapter() const
      { return m_adapter; }

    //inline ConfigManager *GetConfigManager()
    //  { return &m_configManager; }

    QStatus InitializeDevices(bool isUpdate = false);

  private:
    Bridge(std::shared_ptr<adapter::Adapter> const& adapter);

    QStatus RegisterAdapterSignalHandlers(bool isRegister);
    void InitializeAdapter();
    void InitializeInternal();

    void CreateDevice(std::shared_ptr<adapter::Adapter> const& adapter,
      adapter::Device const& device);

    QStatus UpdateDevice(adapter::Device const& device, bool exposedOnAllJoynBus);

    void OnAdapterSignal(adapter::Signal const& signal, void* argp);

  private:
    #if 0
    class AdapterSignalListener : public IAdapterSignalListener
    {
    public:
      AdapterSignalListener(Bridge& parent)
        : m_parent(parent)
        , m_shuttingDown(false)
      {
      }

      virtual void AdapterSignalHandler(IAdapterSignal const& signal, void* argp)
      {
        if (m_shuttingDown)
        {
          return;
        }
        m_parent.OnAdapterSignal(signal, argp);
      }

      void Shutdown()
      {
        m_shuttingDown = true;
      }
      
    private:
      Bridge& m_parent;
      bool m_shuttingDown;
    };
    #endif

  private:
    bool                                      m_alljoynInitialized;
    std::shared_ptr<adapter::Adapter>         m_adapter;
    std::vector<adapter::RegistrationHandle>  m_registeredSignalListeners;
    std::vector< std::shared_ptr<BusObject> > m_bridgeBusObjects;
    ConfigManager                             m_configManager;
  };
}

