#ifndef __DEVICE_METHOD_H__
#define __DEVICE_METHOD_H__

#include "Bridge/IAdapter.h"
#include "Bridge/AllJoynHeaders.h"


namespace bridge
{
  class DeviceMain;

  class DeviceMethod
  {
  public:
    DeviceMethod(DeviceMain&);
    virtual ~DeviceMethod();

    QStatus Initialize(std::shared_ptr<IAdapterMethod> const& adapterMethod);

    // TODO: error code?
    // TODO: should that first arg be constant?
    uint32_t InvokeMethod(ajn::Message& msg, ajn::MsgArg* outArgs, size_t* numOutArgs);

    inline std::string const& GetName() const
      { return m_exposedName; }

  private:
    QStatus SetName(std::string const& name);
    QStatus BuildSignature(AdapterValueVector const& valueList, std::string& sig, std::string& parameterNames);

  private:
    DeviceMain&                     m_parent;
    std::string                     m_exposedName;
    std::string                     m_inSignature;
    std::string                     m_outSignature;
    std::string                     m_parameterNames;
    std::shared_ptr<IAdapterMethod>      m_adapterMethod;
  };
}

#endif
