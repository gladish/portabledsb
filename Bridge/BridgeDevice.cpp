#include "BridgeDevice.h"

#include "AllJoynHelper.h"
#include "DsbServiceNames.h"
#include "Common/Log.h"

namespace
{
  DSB_DECLARE_LOGNAME(BridgeDevice);
}

bridge::BridgeDevice::BridgeDevice(const std::shared_ptr<IAdapterDevice>& device, const std::shared_ptr<IAdapter>& adapter)
  : m_device(device)
  , m_adapter(adapter)
  , m_busAttachment(AllJoynHelper::EncodeStringForAppName(adapter->GetExposedApplicationName()).c_str(), true)
  , m_deviceMain(*this, device)
{
}

bridge::BridgeDevice::~BridgeDevice()
{
  Shutdown();
}

QStatus
bridge::BridgeDevice::Shutdown()
{
  RegisterSignalHandlers(false);
  m_about.Shutdown();

  // TODO: Shutdown icon, control panel, device properties, interface

  ShutdownAllJoyn();

  m_serviceName.clear();

  return ER_OK;
}

QStatus
bridge::BridgeDevice::Initialize()
{
  QStatus st;
  ajn::InterfaceDescription* interface;
  
  // create Device service name
  st = BuildServiceName();
  if (st != ER_OK)
    goto leave;

  // init alljoyn
  st = InitializeAllJoyn();
  if (st != ER_OK)
    goto leave;

  // initialize about service
  st = m_about.Initialize(m_busAttachment);
  if (st != ER_OK)
    goto leave;

  // set device info in about
  m_about.SetApplicationName(m_adapter->GetExposedApplicationName().c_str());
  m_about.SetApplicationGuid(m_adapter->GetExposedApplicationGuid());
  m_about.SetDeviceName(m_device->GetName().c_str());
  m_about.SetManufacturer(m_device->GetVendor().c_str());
  m_about.SetModel(m_device->GetModel().c_str());
  m_about.SetVersion(m_device->GetVersion().c_str());
  m_about.SetDeviceId(m_device->GetSerialNumber().c_str());
  m_about.SetDescription(m_device->GetDescription().c_str());


  // TODO: If an Icon is available, try to add it to the bus attachment too.  Just continue on error
  //if (m_device->GetIcon() != NULL)
  //  m_icon.Initialize(m_busAttachment, m_device->GetIcon());

  // create device properties
  st = CreateDeviceProperties();
  if (st != ER_OK)
    goto leave;

  // create main device
  st = m_deviceMain.Initialize();
  if (st != ER_OK)
    goto leave;

  // Create a control panel if requested by the caller.
  st = InitControlPanel();
  if (st != ER_OK)
    goto leave;
  
  // Create Lighting Service if requested
  st = InitLightingService();
  if (st != ER_OK)
    goto leave;

  interface = m_deviceMain.GetInterfaceDescription();
  if (!interface)
  {
    DSBLOG_WARN("No interface description for DeviceMain");
    return ER_FAIL;
  }
  m_about.AddObject(m_deviceMain, *interface);

  // connect to AllJoyn
  st = ConnectToAllJoyn();
  if (st != ER_OK)
    goto leave;

  // register signals
  st = RegisterSignalHandlers(true);
  if (st != ER_OK)
    goto leave;

  // announce
  m_about.Announce();

leave:
  if (st != ER_OK)
    Shutdown();
  return st;
}

QStatus
bridge::BridgeDevice::BuildServiceName()
{
  m_rootStringForAllJoynNames.clear();
  m_serviceName.clear();

  // set root/prefix for AllJoyn service name (aka bus name) and interface names :
  // 'prefixForAllJoyn'.'AdapterName'.'DeviceName'
  std::string tmp = AllJoynHelper::EncodeStringForRootServiceName(m_adapter->GetExposedAdapterPrefix());
  if (tmp.empty())
    return ER_BUS_BAD_BUS_NAME;
  m_rootStringForAllJoynNames = tmp;

  tmp = AllJoynHelper::EncodeStringForServiceName(m_adapter->GetAdapterName());
  if (tmp.empty())
    return ER_BUS_BAD_BUS_NAME;
  m_rootStringForAllJoynNames += ".";
  m_rootStringForAllJoynNames += tmp;

  // set service name (aka bus name)
  m_serviceName = m_rootStringForAllJoynNames;

  //add device name
  tmp = AllJoynHelper::EncodeStringForServiceName(m_device->GetName());
  if (!tmp.empty())
  {
    m_serviceName += ".";
    m_serviceName += tmp;
  }

  // add serial number to service name if not empty
  tmp = AllJoynHelper::EncodeStringForServiceName(m_device->GetSerialNumber());
  if (!tmp.empty())
  {
    m_serviceName += ".";
    m_serviceName += tmp;
  }

  return ER_OK;
}

QStatus
bridge::BridgeDevice::CreateDeviceProperties()
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_OK;
}

QStatus
bridge::BridgeDevice::RegisterSignalHandlers(bool)
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_OK;
}

QStatus
bridge::BridgeDevice::InitializeAllJoyn()
{
  m_busAttachment.RegisterBusListener(*this);

  QStatus st = m_busAttachment.Start();
  if (st != ER_OK)
    return st;

  // TODO: Initialize authentication handler

  return st;
}

QStatus
bridge::BridgeDevice::ConnectToAllJoyn()
{
  QStatus st = m_busAttachment.Connect();
  if (st != ER_OK)
    return st;

  /*
  * Advertise this service on the bus.
  * There are three steps to advertising this service on the bus.
  * 1) Request a well-known name that will be used by the client to discover
  *    this service.
  * 2) Create a session.
  * 3) Advertise the well-known name.
  */
  st = m_busAttachment.RequestName(m_serviceName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE);
  if (st != ER_OK)
    return st;

  ajn::SessionOpts sessionOpts(ajn::SessionOpts::TRAFFIC_MESSAGES, true, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
  ajn::SessionPort sessionPort = DSB_SERVICE_PORT;
  st = m_busAttachment.BindSessionPort(sessionPort, sessionOpts, *this);
  if (st != ER_OK)
    return st;

  st = m_busAttachment.AdvertiseName(m_serviceName.c_str(), sessionOpts.transports);
  if (st != ER_OK)
    return st;

  return ER_OK;
}

QStatus
bridge::BridgeDevice::ShutdownAllJoyn()
{
  if (!m_serviceName.empty())
    m_busAttachment.CancelAdvertiseName(m_serviceName.c_str(), ajn::TRANSPORT_ANY);

  m_busAttachment.UnbindSessionPort(DSB_SERVICE_PORT);

  if (!m_serviceName.empty())
    m_busAttachment.ReleaseName(m_serviceName.c_str());

  m_busAttachment.Disconnect();

  //TODO: m_authHandler.Shutdown();

  m_busAttachment.Stop();
  return ER_OK;
}

QStatus
bridge::BridgeDevice::InitControlPanel()
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_OK;
}

QStatus
bridge::BridgeDevice::InitLightingService()
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_OK;
}
