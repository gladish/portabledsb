
#include "AllJoynHelper.h"
#include "DeviceMain.h"


QStatus
bridge::AllJoynHelper::SetMsgArg(IAdapterValue const& adapterValue, ajn::MsgArg& m)
{
  QStatus st = ER_OK;
  std::string sig;
  common::Variant const& val = adapterValue.GetData();

  st = GetSignature(val.GetType(), sig);
  if (st != ER_OK)
    return st;

  switch (val.GetType())
  {
    case common::Variant::DataType::Null:
    break;

    case common::Variant::DataType::Boolean:
    {
      bool b = val.ToBoolean();
      st = m.Set(sig.c_str(), &b);
    }
    break;

    case common::Variant::DataType::UInt8:
    {
      uint8_t i = val.ToUInt8();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case common::Variant::DataType::Int16:
    {
      int16_t i = val.ToInt16();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case common::Variant::DataType::UInt16:
    {
      uint16_t u = val.ToUInt16();
      st = m.Set(sig.c_str(), &u);
    }
    break;

    case common::Variant::DataType::Int32:
    {
      int32_t i = val.ToInt32();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case common::Variant::DataType::UInt32:
    {
      uint32_t u = val.ToUInt32();
      st = m.Set(sig.c_str(), &u);
    }
    break;

    case common::Variant::DataType::Int64:
    {
      int64_t l = val.ToInt64();
      st = m.Set(sig.c_str(), &l);
    }
    break;

    case common::Variant::DataType::UInt64:
    {
      uint64_t l = val.ToUInt64();
      st = m.Set(sig.c_str(), &l);
    }
    break;

    case common::Variant::DataType::Double:
    {
      double d = val.ToDouble();
      st = m.Set(sig.c_str(), &d);
    }
    break;

    case common::Variant::DataType::String:
    {
      std::string s = val.ToString();
      if (!s.empty())
      {
        st = m.Set(sig.c_str(), s.c_str());
        if (st == ER_OK)
          m.Stabilize();
      }
      else
      {
        st = m.Set(sig.c_str(), "");
      }
    }
    break;

    case common::Variant::DataType::BooleanArray:
      /* Using vector<uint8_t> instead of vector<bool>, since vector<bool> is a
       * bitfield and we can't access the underlying array easily.
       * http://en.cppreference.com/w/cpp/container/vector_bool */
      st = SetMsgArg<uint8_t>(m, sig.c_str(), val.ToUInt8Array());
    break;

    case common::Variant::DataType::UInt8Array:
      st = SetMsgArg<uint8_t>(m, sig.c_str(), val.ToUInt8Array());
    break;

    case common::Variant::DataType::Int16Array:
      st = SetMsgArg<int16_t>(m, sig.c_str(), val.ToInt16Array());
    break;

    case common::Variant::DataType::UInt16Array:
      st = SetMsgArg<uint16_t>(m, sig.c_str(), val.ToUInt16Array());
    break;

    case common::Variant::DataType::Int32Array:
      st = SetMsgArg<int32_t>(m, sig.c_str(), val.ToInt32Array());
    break;

    case common::Variant::DataType::UInt32Array:
      st = SetMsgArg<uint32_t>(m, sig.c_str(), val.ToUInt32Array());
    break;

    case common::Variant::DataType::Int64Array:
      st = SetMsgArg<int64_t>(m, sig.c_str(), val.ToInt64Array());
    break;

    case common::Variant::DataType::UInt64Array:
      st = SetMsgArg<uint64_t>(m, sig.c_str(), val.ToUInt64Array());
    break;

    case common::Variant::DataType::DoubleArray:
      st = SetMsgArg<double>(m, sig.c_str(), val.ToDoubleArray());
    break;

    case common::Variant::DataType::StringArray:
      st = SetMsgArg<std::string>(m, sig.c_str(), val.ToStringArray());
    break;
  }

  return st;
}

template<>
QStatus bridge::AllJoynHelper::SetMsgArg(ajn::MsgArg& msg, std::string const& sig, std::vector<std::string> const& arr)
{
  QStatus st = ER_OK;

  if (!arr.empty())
  {
    int n = static_cast<int>(arr.size());

    typedef char const* value_type;
    value_type* p = new value_type[n];

    for (int i = 0; i < n; ++i)
      p[i] = arr[i].c_str();

    st = msg.Set(sig.c_str(), n, p);
    msg.Stabilize();
  }
  else
  {
    st = msg.Set(sig.c_str(), 1, "");
    msg.Stabilize();
  }

  return st;
}
   
QStatus
bridge::AllJoynHelper::SetMsgArgFromAdapterObject(IAdapterValue const& adapterValue, ajn::MsgArg&, DeviceMain*)
{
  // TODO:
  QStatus st = ER_OK;
  common::Variant const& val = adapterValue.GetData();
  QCC_UNUSED(val);
  // std::string path = deviceMain->GetBusObjectPath(adapterValue);

  return st;
}

QStatus
bridge::AllJoynHelper::GetAdapterValue(IAdapterValue& adapterValue, ajn::MsgArg const& msg)
{
  QStatus                 st;
  std::string             sig;
  common::Variant const&  val = adapterValue.GetData();

  // TODO: why not just hard-code signatures? GetSignature is never used outside
  // AllJoynHelper
  st = GetSignature(val.GetType(), sig);
  if (st != ER_OK)
    return st;

  switch (val.GetType())
  {
    case common::Variant::DataType::Boolean:
      {
        bool b;
        st = msg.Get(sig.c_str(), &b);
        if (st == ER_OK)
          adapterValue.SetData(common::Variant(b));
      }
      break;

    // TODO: reset of enumerations

    default:
      st = ER_NOT_IMPLEMENTED;
  }

  return st;
}

QStatus
bridge::AllJoynHelper::GetAdapterObject(IAdapterValue&, ajn::MsgArg const&, DeviceMain*)
{
  return ER_NOT_IMPLEMENTED;
}

QStatus
bridge::AllJoynHelper::GetSignature(common::Variant::DataType type, std::string& sig)
{
  #define setSignature(T, S) case common::Variant::DataType::T: sig = S; break

  QStatus status = ER_OK;
  switch (type)
  {
    setSignature(Boolean,     "b");
    setSignature(UInt8,       "y");
    setSignature(Int16,       "n");
    setSignature(UInt16,      "q");
    setSignature(Int32,       "i");
    setSignature(UInt32,      "u");
    setSignature(Int64,       "x");
    setSignature(UInt64,      "t");
    setSignature(Double,      "d");
    setSignature(String,      "s");

    // arrays
    setSignature(BooleanArray,"ab");
    setSignature(UInt8Array,  "ay");
    setSignature(Int16Array,  "an");
    setSignature(UInt16Array, "aq");
    setSignature(Int32Array,  "ai");
    setSignature(UInt32Array, "au");
    setSignature(Int64Array,  "ax");
    setSignature(UInt64Array, "at");
    setSignature(DoubleArray, "ad");
    setSignature(StringArray, "as");

    default:
      status = ER_NOT_IMPLEMENTED;
      break;
  }

  #undef setSignature

  return status;
}


void
bridge::AllJoynHelper::EncodeBusObjectName(std::string const& s, std::string &t)
{
  t.clear();

  for (char ch : s)
  {
    if (std::isalnum(ch))
      t += ch;
    else if (std::isspace(ch) || ch == '_')
      t += '_';
    else if (ch == '.' || ch == '/')
      t += '/';
  }

  t = TrimChar(t, '/');
}

void
bridge::AllJoynHelper::EncodePropertyOrMethodOrSignalName(std::string const& s, std::string &t)
{
  t.clear();

  bool upperCaseNextChar = true;
  bool is1stChar = true;

  for (char ch : s)
  {
    if ((std::isalnum(ch) && !is1stChar) || (std::isalpha(ch) && is1stChar))
    {
      if (upperCaseNextChar && std::isalpha(ch) && std::islower(ch))
        t += std::toupper(ch);
      else
        t += ch;
      upperCaseNextChar = false;
      is1stChar = false;
    }
    else
    {
      upperCaseNextChar = true;
    }
  }
}

void
bridge::AllJoynHelper::EncodeStringForInterfaceName(std::string const& s, std::string& t)
{
  t.clear();
  
  for (char ch : s)
  {
    if (std::isalnum(ch) || ch == '.')
      t += ch;
  }
  t = TrimChar(t, '.');
}

std::string
bridge::AllJoynHelper::EncodeStringForServiceName(std::string const& s)
{
  std::string t;
  std::string temp;

  for (char ch : s)
  {
    if (std::isalpha(ch))
      temp += ch;
  }

  if (!temp.empty())
  {
    if (std::isdigit(temp[0]))
      t += '_';
    t += temp;
  }
  return t;
}

std::string
bridge::AllJoynHelper::EncodeStringForRootServiceName(std::string const& s)
{
  std::string t;
  char curr = '\0';

  for (char ch : s)
  {
    if (std::isalpha(ch) || ch == '.')
    {
      t += ch;
      curr = ch;
    }
    else if (std::isdigit(ch))
    {
      if (curr == '.')
        t += '_';
      t += ch;
      curr = ch;
    }
  }

  t = TrimChar(t, '.');
  return t;
}

std::string
bridge::AllJoynHelper::EncodeStringForAppName(std::string const& s)
{
  std::string t;
  for (char ch : s)
  {
    if (std::isalnum(ch))
      t += ch;
  }
  return t;
}

std::string
bridge::AllJoynHelper::TrimChar(std::string const& s, char c)
{
  std::size_t left = s.find_first_not_of(c);
  if (left == std::string::npos)
    left = 0;

  std::size_t right = s.find_last_not_of(c);

  // TODO: error/bounds check

  return s.substr(left, right - left + 1);
}
