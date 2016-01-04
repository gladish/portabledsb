#include "BridgeDevice.h"

#include "AllJoynHelper.h"

bridge::BridgeDevice::BridgeDevice(const shared_ptr<IAdapterDevice>& dev, const shared_ptr<IAdapter>& adapter)
  : m_parent(dev)
  , m_busAttachment(AllJoynHelper::EncodeStringForAppName(adapter->GetExposedApplicationName()).c_str(), true)
{
}

bridge::BridgeDevice::~BridgeDevice()
{
  Shutdown();
}

QStatus
bridge::BridgeDevice::Shutdown()
{
  QStatus st = ER_OK;
  return st;
}

QStatus
bridge::BridgeDevice::Initialize()
{
  QStatus st = ER_OK;
  return st;
}

