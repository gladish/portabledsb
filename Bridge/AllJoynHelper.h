#pragma once

#include "Common/AdapterNamedValue.h"

#include <string>
#include <vector>
#include <alljoyn/MsgArg.h>

namespace bridge
{
  class AllJoynHelper
  {
  public:
    static adapter::TypeId GetType(ajn::MsgArg const& msgArg);

    static QStatus ValueToMsgArg(adapter::Value const& v, ajn::MsgArg& m);
    
    template<class T>
    static QStatus SetMsgArg(ajn::MsgArg& msgArg, std::string const& sig, std::vector<T> const& arr);

    template<class T>
    static QStatus GetArrayFromMsgArg(ajn::MsgArg& msgArg, std::string const& sig, std::vector<T>& arr);

    
    // static QStatus SetMsgArgFromAdapterObject(adapter::Value const& adapterValue, ajn::MsgArg& msg);

    static QStatus MsgArgToValue(ajn::MsgArg const& m, adapter::Value& v);

    static std::string GetSignature(adapter::TypeId type);
    static std::string GetSignature(adapter::NamedValue::Vector const& v);
    static std::string GetMethodArgumentNames(adapter::NamedValue::Vector const& in,
        adapter::NamedValue::Vector const& out = adapter::NamedValue::Vector());

    static void EncodeBusObjectName(std::string const& s, std::string &builtName);
    static void EncodePropertyOrMethodOrSignalName(std::string const& s, std::string &builtName);
    static void EncodeStringForInterfaceName(std::string const& s, std::string& encoded);
    static std::string EncodeStringForServiceName(std::string const&);
    static std::string EncodeStringForRootServiceName(std::string const&);
    static std::string EncodeStringForAppName(std::string const&);
    static std::string TrimChar(std::string const& s, char c);
  };

  template<class T>
  QStatus AllJoynHelper::SetMsgArg(ajn::MsgArg& msg, std::string const& sig, std::vector<T> const& arr)
  {
    QStatus st = ER_OK;

    if (!arr.empty())
    {
      st = msg.Set(sig.c_str(), arr.size(), arr.data());
      msg.Stabilize();
    }
    else
    {
      T buff[1];
      buff[0] = T();
      st = msg.Set(sig.c_str(), 1, buff);
      msg.Stabilize();
    }

    return st;
  }

  template<>
  QStatus AllJoynHelper::SetMsgArg(ajn::MsgArg& msg, std::string const& sig, std::vector<std::string> const& arr);
 
}

