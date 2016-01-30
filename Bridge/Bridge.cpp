#include "Bridge/Bridge.h"
#include "Bridge/BridgeError.h"
#include "Bridge/BridgeBusObject.h"

#include "Common/AdapterNamedValue.h"
#include "Common/EnumDeviceOptions.h"
#include "Common/AdapterLog.h"

#include <qcc/Debug.h>
#include <alljoyn/Init.h>

namespace
{
  DSB_DECLARE_LOGNAME(Bridge);

  int32_t const kWaitTimeoutForAdapterOperation = 20000; //ms
  std::string const kDeviceArrivalSignal = "Device_Arrival";
  std::string const kDeviceArrivalDeviceHandle = "Device_Handle";
  std::string const kDeviceRemovalSignal = "Device_Removal";
  std::string const kDeviceRemovalDeviceHandle = "Device_Handle";

  void alljoynLogger(DbgMsgType type, char const* module, char const* msg, void* /*ctx*/)
  {
    static std::string logName = "alljoyn";
    
    adapter::LogLevel level = adapter::LogLevel::Info;
    switch (type)
    {
      case DBG_LOCAL_ERROR:
      case DBG_REMOTE_ERROR:
        level = adapter::LogLevel::Error;
        break;
      case DBG_HIGH_LEVEL:
        level = adapter::LogLevel::Warn;
        break;
      case DBG_GEN_MESSAGE:
        level = adapter::LogLevel::Info;
        break;
      case DBG_API_TRACE:
      case DBG_REMOTE_DATA:
      case DBG_LOCAL_DATA:
        level = adapter::LogLevel::Debug;
    }

    adapter::Log::GetLog(logName)->Write(level, NULL, 0, "[%s] %s", module, msg);
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

  std::shared_ptr<bridge::Bridge> gBridge;
}

std::shared_ptr<bridge::Bridge>
bridge::Bridge::GetInstance()
{
  return gBridge;
}

void
bridge::Bridge::InitializeSingleton(std::shared_ptr<adapter::Adapter> const& adapter)
{
  gBridge.reset(new Bridge(adapter));
}

void
bridge::Bridge::ReleaseSingleton()
{
  gBridge.reset();
}


bridge::Bridge::Bridge(std::shared_ptr<adapter::Adapter> const& adapter)
  : m_alljoynInitialized(false)
  , m_adapter(adapter)
  , m_configManager(*this, adapter)
{
  // TODO: bug in alljoyn prevents registering logger.
  // RegisterAllJoynLogger();
}

bridge::Bridge::~Bridge()
{
  try
  {
    Shutdown();
  }
  catch (std::exception const& err)
  {
    DSBLOG_WARN("error shutting down bridge: %s", err.what());
  }
}

void
bridge::Bridge::Initialize()
{
  DSBLOG_DEBUG(__FUNCTION__);

  QStatus st = ER_OK;
  if (!m_alljoynInitialized)
  {
    DSBLOG_INFO("initialize AllJoyn");
    st = AllJoynInit();
    Error::ThrowIfNotOk(st, "AllJoyn init failed");
  }

  InitializeInternal();
  m_alljoynInitialized = true;
}

void
bridge::Bridge::InitializeInternal()
{
  QStatus st = ER_OK;

  DSBLOG_INFO("initialize configuration manager");
  st = m_configManager.Initialize(), 
  Error::ThrowIfNotOk(st, "failed to initialize configuration manager");

  DSBLOG_INFO("initialize adapter");
  InitializeAdapter();

  DSBLOG_INFO("connect to AllJoyn router");
  st = m_configManager.ConnectToAllJoyn();
  Error::ThrowIfNotOk(st, "error connecting to router");

  DSBLOG_INFO("initialize devices");
  InitializeDevices();

  DSBLOG_INFO("registering signal handlers");
  st = RegisterAdapterSignalHandlers(true);
  Error::ThrowIfNotOk(st, "failed to register signal handlers");
}

void
bridge::Bridge::Shutdown()
{
  m_bridgeBusObjects.clear();

  if (m_adapter)
  {
    std::shared_ptr<adapter::IoRequest> req(new adapter::IoRequest());

    RegisterAdapterSignalHandlers(false);
    m_adapter->Shutdown(req);
    req->Wait();
  }

  m_configManager.Shutdown();
  
  if (m_alljoynInitialized)
  {
    AllJoynShutdown();
    m_alljoynInitialized = false;
  }
}

void
bridge::Bridge::InitializeAdapter()
{
  if (!m_adapter)
    Error::Throw("can't initialize null adapter");

  std::shared_ptr<adapter::IoRequest> req(new adapter::IoRequest());

  adapter::ItemInformation info = m_adapter->GetInfo();

  req.reset(new adapter::IoRequest());
  std::shared_ptr<adapter::Log> log = adapter::Log::GetLog(info.GetName());

  uint32_t ret = m_adapter->Initialize(log, req);
  if (ret)
    Error::Throw(*m_adapter, ret, "failed to initialize adapter");

  if (!req->Wait(2000))
  {
    DSBLOG_WARN("timeout waiting for adapter to initailize");
  }
  else
  {
    ret = req->GetStatus();
    if (ret)
      Error::Throw(*m_adapter, ret, "failed to async intiailize adapter");
  }
}

void
bridge::Bridge::InitializeDevices(bool update)
{
  adapter::EnumDeviceOptions opts = adapter::EnumDeviceOptions::CacheOnly;
  if (update)
    opts = adapter::EnumDeviceOptions::ForceRefresh;

 
  DSBLOG_DEBUG("enumerating devices from adapter");

  adapter::Device::Vector devices;
  adapter::IoRequest::Pointer req(new adapter::IoRequest());

  adapter::Status st = m_adapter->EnumDevices(opts, devices, req);
  if (st != 0)
    Error::Throw(*m_adapter, st, "failed to enumerate devices");

  if (!req->Wait(kWaitTimeoutForAdapterOperation))
    Error::Throw("timeout waiting for adapter to initialize");

  for (auto device : devices)
  {
    // TODO: get configuration for device, only expose visible devices
    if (update)
      UpdateDevice(device, true);
    else
      CreateDevice(m_adapter, device);
  }

  // TODO: Save bridge configuration to XML
}

void
bridge::Bridge::OnAdapterSignal(adapter::Signal const& signal, void*)
{
  #if 0
  // TODO
  std::shared_ptr<IAdapterDevice> adapterDevice;

  std::string const name = signal.GetName();
  if (name == kDeviceArrivalSignal || name == kDeviceRemovalSignal)
  {
    adapter::AdapterValue::Vector params = signal.GetParams();
    for (auto const& param: params)
    {
      std::string const& paramName = param.GetName();
      if (paramName == kDeviceArrivalDeviceHandle || paramName == kDeviceRemovalDeviceHandle)
      {
        // TODO: figure out where this signal is generated and figure out how we're 
        // going to put an arbitrary pointer inside.
        //
        // adapterDevice = param->GetData();
      }

      if (!adapterDevice)
        return;
    }
  }

  if (name == kDeviceArrivalSignal)
    CreateDevice(adapterDevice);

  else if (name == kDeviceRemovalSignal)
    UpdateDevice(adapterDevice, false);
  #endif
}

QStatus
bridge::Bridge::RegisterAdapterSignalHandlers(bool isRegister)
{
  QStatus st = ER_OK;

  if (isRegister)
  {
      #if 0
    adapter::AdapterSignal::Vector const& signals = m_adapter->GetSignals();
    for (auto const& signal : signals)
    {
        Adapter::RegistrationHandle handle;
        int ret = m_adapter->RegisterSignalListener(signal.GetName(), m_adapterSignalListener, NULL, handle);
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
      #endif
  }
  else
  {
    typedef std::vector<adapter::RegistrationHandle>::const_iterator iterator;
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

void
bridge::Bridge::UpdateDevice(adapter::Device const& dev, bool exposedOnAllJoynBus)
{
  DSBLOG_NOT_IMPLEMENTED();
  DSBLOG_ASSERT_NOT_IMPLEMENTED();
}

void
bridge::Bridge::CreateDevice(std::shared_ptr<adapter::Adapter> const& adapter, adapter::Device const& dev)
{
  std::string appname;
  std::string path = adapter->GetAdapterPrefix() + "/" + dev.GetInfo().GetName();

  DSBLOG_DEBUG("create BusObject for device: %s", dev.GetInfo().GetName().c_str());
  std::shared_ptr<bridge::BusObject> obj = bridge::BusObject::BuildFromAdapterDevice(
    appname, path, adapter, dev);

  // TODO: setup aboutData and announce
  // alljoyn_core/samples/observer/door_provider.cc as example
  obj->Publish();
  
  m_bridgeBusObjects.push_back(obj);
}
