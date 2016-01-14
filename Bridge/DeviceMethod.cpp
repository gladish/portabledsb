#include "DeviceMethod.h"

#include "Common/Log.h"
#include "Bridge/AllJoynHelper.h"
#include "Bridge/Bridge.h"

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
bridge::DeviceMethod::InvokeMethod(ajn::Message& msg, ajn::MsgArg* outArgs, size_t* numOutArgs)
{
  int       i = 0;
  uint32_t  adapterStatus = 0;
  QStatus   status = ER_OK;
  std::shared_ptr<IAdapterIoRequest> request;

  if (outArgs)
    *outArgs = nullptr;

  if (numOutArgs)
    *numOutArgs = 0;

  if (!m_adapterMethod->GetOutputParams().empty())
  {
    if (outArgs == nullptr)
      adapterStatus = 1;
    if (numOutArgs == nullptr)
      adapterStatus = 2;

    // TODO: I don't like all these goto's. Why not just use exception handling?
    if (adapterStatus != 0)
      goto leave;

    // TODO: make sure this gets deleted [] by caller. Should we just use vector with
    // std::shared_ptr<>?
    outArgs = new ajn::MsgArg[m_adapterMethod->GetOutputParams().size()];
    *numOutArgs = m_adapterMethod->GetOutputParams().size();
  }

  for (std::shared_ptr<IAdapterValue> const& param : m_adapterMethod->GetInputParams())
  {
    ajn::MsgArg const* inArg = msg->GetArg(i++);
    if (inArg == nullptr)
    {
      adapterStatus = 1;
      goto leave;
    }

    // TODO: this seems odd. The GetAdapterValue() populates the param argument with the
    // data from the inArg. Why wouldn't we create a temporary based on the incoming
    // ajn::Message? We need the description or prototype to do some validation. I would
    // think the m_adapterMethod would be const. I guess DeviceMethod (this class) can
    // only be used once and then must be discarded, or only used by one thread to
    // completion at any given time
    status = AllJoynHelper::GetAdapterValue(*param, *inArg);
    if (status != ER_OK)
    {
      adapterStatus = 1;
      goto leave;
    }
  }

  // TODO: we're ingoring the return value here
  bridge::DeviceSystemBridge::GetInstance()->GetAdapter()
    ->CallMethod(m_adapterMethod, &request);

  // TODO: timeout should be configurable or passed in by caller
  if (request)
    request->Wait(2000);

  i = 0;
  for (std::shared_ptr<IAdapterValue> const& param : m_adapterMethod->GetOutputParams())
  {
    status = AllJoynHelper::SetMsgArg(*param, outArgs[i]);
    // TODO: check status
  }

  leave:
    if (adapterStatus != 0)
    {
      delete [] outArgs;
      if (numOutArgs)
        *numOutArgs = 0;
    }

  return adapterStatus;
}

QStatus
bridge::DeviceMethod::Initialize(std::shared_ptr<IAdapterMethod> const&)
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

