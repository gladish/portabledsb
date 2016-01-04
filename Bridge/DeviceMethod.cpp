#include "DeviceMethod.h"

#include "Common/Log.h"

namespace
{
  DSB_DECLARE_LOGNAME(DeviceMethod);
}

bridge::DeviceMethod::DeviceMethod(DeviceMain& dev)
  : m_parent(dev)
{
}

bridge::DeviceMethod::~DeviceMethod()
{
}

uint32_t
bridge::DeviceMethod::InvokeMethod(ajn::Message const&, ajn::MsgArg*, size_t)
{
  return 0;
}

QStatus
bridge::DeviceMethod::Initialize(shared_ptr<IAdapterMethod> const&)
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_NOT_IMPLEMENTED;
}

QStatus
bridge::DeviceMethod::SetName(std::string const&)
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_NOT_IMPLEMENTED;
}

QStatus
bridge::DeviceMethod::BuildSignature(AdapterValueVector const&, std::string&, std::string&)
{
  DSBLOG_NOT_IMPLEMENTED();
  return ER_NOT_IMPLEMENTED;
}

