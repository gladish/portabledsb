
#include "AllJoynHelper.h"
#include "Common/Assert.h"

#include <map>

namespace
{
  std::map< adapter::TypeId, std::string > s_typeToSignature = 
  {
    { adapter::TypeId::Boolean, "b" },
    { adapter::TypeId::UInt8  , "y" },
    { adapter::TypeId::Int16  , "n" },
    { adapter::TypeId::UInt16 , "q" },
    { adapter::TypeId::Int32  , "i" },
    { adapter::TypeId::UInt32 , "u" },
    { adapter::TypeId::Int64  , "x" },
    { adapter::TypeId::UInt64 , "t" },
    { adapter::TypeId::Double , "d" },
    { adapter::TypeId::String , "s" },

    { adapter::TypeId::BooleanArray,  "ab" },
    { adapter::TypeId::UInt8Array  ,  "ay" },
    { adapter::TypeId::Int16Array  ,  "an" },
    { adapter::TypeId::UInt16Array ,  "aq" },
    { adapter::TypeId::Int32Array  ,  "ai" },
    { adapter::TypeId::UInt32Array ,  "au" },
    { adapter::TypeId::Int64Array  ,  "ax" },
    { adapter::TypeId::UInt64Array ,  "at" },
    { adapter::TypeId::DoubleArray ,  "ad" },
    { adapter::TypeId::StringArray ,  "as" }
  };

  std::map< std::string, adapter::TypeId > s_signatureToType = 
  {
    { "b", adapter::TypeId::Boolean },
    { "y", adapter::TypeId::UInt8   },
    { "n", adapter::TypeId::Int16   },
    { "q", adapter::TypeId::UInt16  },
    { "i", adapter::TypeId::Int32   },
    { "u", adapter::TypeId::UInt32  },
    { "x", adapter::TypeId::Int64   },
    { "t", adapter::TypeId::UInt64  },
    { "d", adapter::TypeId::Double  },
    { "s", adapter::TypeId::String  },

    { "ab", adapter::TypeId::BooleanArray },
    { "ay", adapter::TypeId::UInt8Array   },
    { "an", adapter::TypeId::Int16Array   },
    { "aq", adapter::TypeId::UInt16Array  },
    { "ai", adapter::TypeId::Int32Array   },
    { "au", adapter::TypeId::UInt32Array  },
    { "ax", adapter::TypeId::Int64Array   },
    { "at", adapter::TypeId::UInt64Array  },
    { "ad", adapter::TypeId::DoubleArray  },
    { "as", adapter::TypeId::StringArray  },
  };
}


QStatus
bridge::AllJoynHelper::SetMsgArg(adapter::Value const& adapterValue, ajn::MsgArg& m)
{
  QStatus st = ER_OK;
  adapter::Variant const& val = adapterValue.GetValue();

  std::string const sig = GetSignature(val.GetType());

  switch (val.GetType())
  {
    case adapter::TypeId::Null:
    break;

    case adapter::TypeId::Boolean:
    {
      bool b = val.ToBoolean();
      st = m.Set(sig.c_str(), &b);
    }
    break;

    case adapter::TypeId::UInt8:
    {
      uint8_t i = val.ToUInt8();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case adapter::TypeId::Int16:
    {
      int16_t i = val.ToInt16();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case adapter::TypeId::UInt16:
    {
      uint16_t u = val.ToUInt16();
      st = m.Set(sig.c_str(), &u);
    }
    break;

    case adapter::TypeId::Int32:
    {
      int32_t i = val.ToInt32();
      st = m.Set(sig.c_str(), &i);
    }
    break;

    case adapter::TypeId::UInt32:
    {
      uint32_t u = val.ToUInt32();
      st = m.Set(sig.c_str(), &u);
    }
    break;

    case adapter::TypeId::Int64:
    {
      int64_t l = val.ToInt64();
      st = m.Set(sig.c_str(), &l);
    }
    break;

    case adapter::TypeId::UInt64:
    {
      uint64_t l = val.ToUInt64();
      st = m.Set(sig.c_str(), &l);
    }
    break;

    case adapter::TypeId::Double:
    {
      double d = val.ToDouble();
      st = m.Set(sig.c_str(), &d);
    }
    break;

    case adapter::TypeId::String:
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

    case adapter::TypeId::BooleanArray:
      /* Using vector<uint8_t> instead of vector<bool>, since vector<bool> is a
       * bitfield and we can't access the underlying array easily.
       * http://en.cppreference.com/w/cpp/container/vector_bool */
      st = SetMsgArg<uint8_t>(m, sig.c_str(), val.ToUInt8Array());
    break;

    case adapter::TypeId::UInt8Array:
      st = SetMsgArg<uint8_t>(m, sig.c_str(), val.ToUInt8Array());
    break;

    case adapter::TypeId::Int16Array:
      st = SetMsgArg<int16_t>(m, sig.c_str(), val.ToInt16Array());
    break;

    case adapter::TypeId::UInt16Array:
      st = SetMsgArg<uint16_t>(m, sig.c_str(), val.ToUInt16Array());
    break;

    case adapter::TypeId::Int32Array:
      st = SetMsgArg<int32_t>(m, sig.c_str(), val.ToInt32Array());
    break;

    case adapter::TypeId::UInt32Array:
      st = SetMsgArg<uint32_t>(m, sig.c_str(), val.ToUInt32Array());
    break;

    case adapter::TypeId::Int64Array:
      st = SetMsgArg<int64_t>(m, sig.c_str(), val.ToInt64Array());
    break;

    case adapter::TypeId::UInt64Array:
      st = SetMsgArg<uint64_t>(m, sig.c_str(), val.ToUInt64Array());
    break;

    case adapter::TypeId::DoubleArray:
      st = SetMsgArg<double>(m, sig.c_str(), val.ToDoubleArray());
    break;

    case adapter::TypeId::StringArray:
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
bridge::AllJoynHelper::SetMsgArgFromAdapterObject(adapter::Value const& adapterValue, ajn::MsgArg&)
{
  // TODO:
  QStatus st = ER_OK;
  adapter::Variant const& val = adapterValue.GetValue();
  QCC_UNUSED(val);
  // std::string path = deviceMain->GetBusObjectPath(adapterValue);

  return st;
}

QStatus
bridge::AllJoynHelper::GetValue(adapter::Value& v, ajn::MsgArg const& msg)
{
  QStatus                 st;
  std::string const sig = ajn::MsgArg::Signature(&msg, 1).c_str();

  auto itr = s_signatureToType.find(sig);
  DSB_ASSERT(itr != s_signatureToType.end());

  adapter::TypeId type = itr->second;

  switch (type)
  {
    case adapter::TypeId::Null:
    break;

    case adapter::TypeId::Boolean:
    {
      bool b;
      if ((st = msg.Get(sig.c_str(), &b)) == ER_OK)
        v.SetValue(b);
    }
    break;

    case adapter::TypeId::Int16:
    {
      int16_t i;
      if ((st = msg.Get(sig.c_str(), &i)) == ER_OK)
        v.SetValue(i);
    }
    break;

    case adapter::TypeId::UInt16:
    {
      uint16_t u;
      if ((st = msg.Get(sig.c_str(), &u)) == ER_OK)
        v.SetValue(u);
    }
    break;

    case adapter::TypeId::Int32:
    {
      int32_t i;
      if ((st = msg.Get(sig.c_str(), &i)) == ER_OK)
        v.SetValue(i);
    }
    break;

    case adapter::TypeId::UInt32:
    {
      uint32_t u;
      if ((st = msg.Get(sig.c_str(), &u)) == ER_OK)
        v.SetValue(u);
    }
    break;

    case adapter::TypeId::Int64:
    {
      int64_t l;
      if ((st = msg.Get(sig.c_str(), &l)) == ER_OK)
        v.SetValue(l);
    }
    break;

    case adapter::TypeId::UInt64:
    {
      uint64_t lu;
      if ((st = msg.Get(sig.c_str(), &lu)) == ER_OK)
        v.SetValue(lu);
    }
    break;

    case adapter::TypeId::Double:
    {
      double d;
      if ((st = msg.Get(sig.c_str(), &d)) == ER_OK)
        v.SetValue(d);
    }
    break;

    case adapter::TypeId::String:
    {
      char* s = nullptr;
      if ((st = msg.Get(sig.c_str(), &s)) == ER_OK)
        v.SetValue(s);
    }
    break;

    // TODO: arrays

    default:
      assert(false);
      st = ER_NOT_IMPLEMENTED;
  }

  return st;
}

QStatus
bridge::AllJoynHelper::GetAdapterObject(adapter::Value&, ajn::MsgArg const&)
{
  return ER_NOT_IMPLEMENTED;
}

std::string
bridge::AllJoynHelper::GetSignature(adapter::TypeId type)
{
  auto itr = s_typeToSignature.find(type);
  if (itr == s_typeToSignature.end())
    DSB_ASSERT(false);
  return itr->second;
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
