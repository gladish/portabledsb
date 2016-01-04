#include "Bridge.h"
#include "Common/Log.h"
#include "Bridge/BridgeDevice.h"
#include <qcc/Debug.h>
#include <alljoyn/Init.h>

namespace
{
  DSB_DECLARE_LOGNAME(DeviceSystemBridge);

  int32_t const kWaitTimeoutForAdapterOperation = 20000; //ms
  std::string const kDeviceArrivalSignal = "Device_Arrival";
  std::string const kDeviceArrivalDeviceHandle = "Device_Handle";
  std::string const kDeviceRemovalSignal = "Device_Removal";
  std::string const kDeviceRemovalDeviceHandle = "Device_Handle";

  inline bridge::BridgeDeviceList::key_type GetKey(shared_ptr<bridge::IAdapterDevice> const& dev)
  {
    return dev.get();
  }

  void alljoynLogger(DbgMsgType type, char const* module, char const* msg, void* /*ctx*/)
  {
    common::Logger::Level level = common::Logger::DSB_LOGLEVEL_INFO;
    switch (type)
    {
      case DBG_LOCAL_ERROR:
      case DBG_REMOTE_ERROR:
        level = common::Logger::DSB_LOGLEVEL_ERROR;
        break;
      case DBG_HIGH_LEVEL:
        level = common::Logger::DSB_LOGLEVEL_WARN;
        break;
      case DBG_GEN_MESSAGE:
        level = common::Logger::DSB_LOGLEVEL_INFO;
        break;
      case DBG_API_TRACE:
      case DBG_REMOTE_DATA:
      case DBG_LOCAL_DATA:
        level = common::Logger::DSB_LOGLEVEL_DEBUG;
    }
    common::Logger::Write("alljoyn", level, NULL, 0, "[%s] %s", module, msg);
  }

  void RegisterAllJoynLogger()
  {
    static bool alljoynLoggerRegistered = false;
    if (!alljoynLoggerRegistered)
    {
      alljoynLoggerRegistered = true;
      QCC_RegisterOutputCallback(alljoynLogger, NULL);
    }
  }
}

bridge::DeviceSystemBridge::DeviceSystemBridge(shared_ptr<IAdapter> const& adapter)
  : m_alljoynInitialized(false)
  , m_adapter(adapter)
  , m_adapterSignalListener(new AdapterSignalListener(*this))
  , m_configManager(*this, *adapter)
{
  // TODO: bug in alljoyn prevents registering logger.
  // RegisterAllJoynLogger();
}

bridge::DeviceSystemBridge::~DeviceSystemBridge()
{
  m_adapterSignalListener->Shutdown();
  Shutdown();
  m_adapter.reset();
}

QStatus
bridge::DeviceSystemBridge::Initialize()
{
  QStatus st = ER_OK;

  DSBLOG_DEBUG(__FUNCTION__);
  if (!m_alljoynInitialized)
  {
    DSBLOG_INFO("initialize AllJoyn");
    st = AllJoynInit();
    if (st != ER_OK)
    {
      DSBLOG_WARN("initialize AllJoyn failed: %s", QCC_StatusText(st));
      goto Leave;
    }

    m_alljoynInitialized = true;
  }

  st = InitializeInternal();
  if (st != ER_OK)
    goto Leave;

Leave:
  if (st != ER_OK)
  {
    Shutdown();
  }

  return st;
}

QStatus
bridge::DeviceSystemBridge::InitializeInternal()
{
  QStatus st = ER_OK;

  DSBLOG_INFO("initialize configuration manager");
  st = m_configManager.Initialize();

  if (st != ER_OK)
  {
    DSBLOG_WARN("initialize configuration manager failed: %s", QCC_StatusText(st));
    goto Leave;
  }

  DSBLOG_INFO("initialize adapter");
  st = InitializeAdapter();

  if (st != ER_OK)
  {
    DSBLOG_WARN("initialize adapter failed: %s", QCC_StatusText(st));
    goto Leave;
  }

  DSBLOG_INFO("connect to AllJoyn router");
  st = m_configManager.ConnectToAllJoyn();

  if (st != ER_OK)
  {
    DSBLOG_INFO("connect to AllJoyn router failed: %s", QCC_StatusText(st));
    goto Leave;
  }

  DSBLOG_INFO("initialize devices");
  st = InitializeDevices();
  if (st != ER_OK)
  {
    DSBLOG_WARN("initialize devices failed: %s", QCC_StatusText(st));
    goto Leave;
  }

  DSBLOG_INFO("registering signal handlers");
  st = RegisterAdapterSignalHandlers(true);
  if (st != ER_OK)
  {
    DSBLOG_WARN("register signal handlers failed: %s", QCC_StatusText(st));
    goto Leave;
  }

Leave:
  return st;
}

QStatus
bridge::DeviceSystemBridge::Shutdown()
{
  QStatus st = ER_OK;

  st = ShutdownInternal();
  if (st != ER_OK)
  {
    DSBLOG_WARN("failed to shutdown internal: %s", QCC_StatusText(st));
    return st;
  }

  if (m_alljoynInitialized)
  {
    AllJoynShutdown();
    m_alljoynInitialized = false;
  }

  return st;
}

QStatus
bridge::DeviceSystemBridge::InitializeAdapter()
{
  if (!m_adapter)
  {
    DSBLOG_ERROR("can't initialize null adapter");
    return ER_FAIL;
  }
  int ret = m_adapter->Initialize();
  return ret == 0 ? ER_OK : ER_FAIL;
}

QStatus
bridge::DeviceSystemBridge::InitializeDevices(bool update)
{
  AdapterDeviceVector deviceList;
  shared_ptr<IAdapterIoRequest> request;

  EnumDeviceOptions opts = EnumDeviceOptions::CacheOnly;
  if (update)
    opts = EnumDeviceOptions::ForceRefresh;

  int ret = m_adapter->EnumDevices(opts, deviceList, &request);

  if (ret != 0)
    goto Leave;

  if (request)
  {
    ret = request->Wait(kWaitTimeoutForAdapterOperation);
    if (ret != 0)
    {
      goto Leave;
    }
  }

  typedef AdapterDeviceVector::iterator iterator;
  for (iterator begin = deviceList.begin(), end = deviceList.end(); begin != end; ++begin)
  {
    // TODO: get configuration for device, only expose visible devices
    if (update)
      ret = UpdateDevice(*begin, true);
    else
      ret = CreateDevice(*begin);

    if (ret != 0)
      DSBLOG_WARN("Failed to %s device: %d", update ? "update" : "create", ret);
  }

  // TODO: Save bridge configuration to XML

Leave:
  return ret == 0 ? ER_OK : ER_FAIL;
}

void
bridge::DeviceSystemBridge::OnAdapterSignal(IAdapterSignal const& signal, void*)
{
  // TODO
  shared_ptr<IAdapterDevice> adapterDevice;

  std::string const name = signal.GetName();
  if (name == kDeviceArrivalSignal || name == kDeviceRemovalSignal)
  {
    AdapterValueVector params = signal.GetParams();
    for (AdapterValueVector::const_iterator itr = params.begin(); itr != params.end(); ++itr)
    {
      const shared_ptr<IAdapterValue>& param = (*itr);
      const std::string& paramName = param->GetName();
      if (paramName == kDeviceArrivalDeviceHandle || paramName == kDeviceRemovalDeviceHandle)
      {
        // TODO: figure out where this signal is generated and figure out how we're 
        // going to put an arbitrary pointer inside.
        //
        // adapterDevice = param->GetData();
      }

      if (!adapterDevice)
        goto Leave;
    }
  }

  if (name == kDeviceArrivalSignal)
  {
    CreateDevice(adapterDevice);
  }
  else if (name == kDeviceRemovalSignal)
  {
    UpdateDevice(adapterDevice, false);
  }
Leave:
  return;
}

QStatus
bridge::DeviceSystemBridge::RegisterAdapterSignalHandlers(bool isRegister)
{
  QStatus st = ER_OK;

  if (isRegister)
  {
    AdapterSignalVector signals = m_adapter->GetSignals();
    for (AdapterSignalVector::const_iterator itr = signals.begin(); itr != signals.end(); ++itr)
    {
        IAdapter::RegistrationHandle handle;
        int ret = m_adapter->RegisterSignalListener((*itr)->GetName(), m_adapterSignalListener, NULL, handle);
        if (ret != 0)
        {
          DSBLOG_WARN("failed to register signal listener on adapter: %s", QCC_StatusText(st));
          if (st == ER_OK)
            ret = st;
        }
        else
        {
          m_registeredSignalListeners.push_back(handle);
        }
      }
  }
  else
  {
    typedef std::vector<IAdapter::RegistrationHandle>::const_iterator iterator;
    for (iterator begin = m_registeredSignalListeners.begin(), end = m_registeredSignalListeners.end();
      begin != end; ++begin)
    {
      int ret = m_adapter->UnregisterSignalListener(*begin);
      if (ret != 0)
      {
        DSBLOG_WARN("failed to unregister signal listener on adapter: %s", QCC_StatusText(st));
        if (st == ER_OK)
          st = ER_FAIL;
      }
    }
  }

  return st;
}

QStatus
bridge::DeviceSystemBridge::ShutdownInternal()
{
  m_configManager.Shutdown();

  if (m_adapter)
  {
    RegisterAdapterSignalHandlers(false);
    m_adapter->Shutdown();
  }

  for (BridgeDeviceList::iterator itr = m_deviceList.begin(); itr != m_deviceList.end(); ++itr)
    itr->second->Shutdown();

  m_deviceList.clear();

  return ER_OK;
}

QStatus
bridge::DeviceSystemBridge::UpdateDevice(shared_ptr<IAdapterDevice> const& dev, bool exposedOnAllJoynBus)
{
  QStatus st = ER_OK;

  BridgeDeviceList::const_iterator itr = m_deviceList.find(GetKey(dev));
  if (itr == m_deviceList.end() && exposedOnAllJoynBus)
  {
    st = CreateDevice(dev);
  }
  else if (itr != m_deviceList.end() && !exposedOnAllJoynBus)
  {
    m_deviceList.erase(itr);
    if ((st = itr->second->Shutdown()) != ER_OK)
      DSBLOG_WARN("failed to shutdown BridgeDevice: %s", QCC_StatusText(st));
  }

  return st;
}

QStatus
bridge::DeviceSystemBridge::CreateDevice(shared_ptr<IAdapterDevice> const& dev)
{
  if (!dev.get())
    return ER_BAD_ARG_1;

  shared_ptr<BridgeDevice> newDevice(new BridgeDevice(dev, m_adapter));
  
  QStatus st = newDevice->Initialize();
  if (st == ER_OK)
    m_deviceList.insert(std::make_pair(GetKey(dev), newDevice));

  return st;
}
