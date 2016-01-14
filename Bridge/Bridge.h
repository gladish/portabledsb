#pragma once

#include "Bridge/IAdapter.h"
#include "Bridge/ConfigManager.h"

#include <alljoyn/Status.h>

#include <map>
#include <string>

namespace bridge
{
  class IAdapter;
  class BridgeDevice;

  typedef std::map<IAdapterDevice*, std::shared_ptr<BridgeDevice> > BridgeDeviceList;

  class DeviceSystemBridge : public std::enable_shared_from_this<DeviceSystemBridge>
  {
  public:
    static std::shared_ptr<DeviceSystemBridge> GetInstance();
    static void InitializeSingleton(std::shared_ptr<IAdapter> const& adapter);

    virtual ~DeviceSystemBridge();

    QStatus Initialize();
    QStatus Shutdown();

    inline std::shared_ptr<IAdapter> GetAdapter() const
      { return m_adapter; }

    //inline ConfigManager *GetConfigManager()
    //  { return &m_configManager; }

    QStatus InitializeDevices(bool isUpdate = false);

    inline BridgeDeviceList GetDeviceList() const
      { return m_deviceList; }

  private:
    DeviceSystemBridge(std::shared_ptr<IAdapter> const& adapter);

    QStatus RegisterAdapterSignalHandlers(bool isRegister);
    QStatus InitializeAdapter();
    QStatus InitializeInternal();
    QStatus ShutdownInternal();

    QStatus CreateDevice(std::shared_ptr<IAdapterDevice> const& device);
    QStatus UpdateDevice(std::shared_ptr<IAdapterDevice> const& device, bool exposedOnAllJoynBus);

    void OnAdapterSignal(IAdapterSignal const& signal, void* argp);

  private:
    class AdapterSignalListener : public IAdapterSignalListener
    {
    public:
      AdapterSignalListener(DeviceSystemBridge& parent)
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
      DeviceSystemBridge& m_parent;
      bool m_shuttingDown;
    };

  private:
    bool                                      m_alljoynInitialized;
    std::shared_ptr<IAdapter>                      m_adapter;
    BridgeDeviceList                          m_deviceList;
    std::shared_ptr<AdapterSignalListener>         m_adapterSignalListener;
    std::vector<IAdapter::RegistrationHandle> m_registeredSignalListeners;
    ConfigManager m_configManager;
  };
}

