//
// Copyright (c) 2016, Comcast Cable Communications Management, LLC
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or
// without fee is hereby granted, provided that the above copyright notice and this
// permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
// SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
// ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.
//
#include "AllJoynHelper.h"
#include "common/Assert.h"

#include <map>
#include <sstream>

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

  template<class T>
  QStatus Extract(ajn::MsgArg const& m, std::string const& sig, adapter::Value& v)
  {
    T val;
    QStatus st = m.Get(sig.c_str(), &val);
    if (st == ER_OK)
      v = val;
    return st;
  }
}


template<>
QStatus bridge::AllJoynHelper::SetMsgArg(ajn::MsgArg& msg, std::string const& sig, std::vector<std::string> const& arr)
{
  QStatus st = ER_OK;

  if (!arr.empty())
  {
    int n = static_cast<int>(arr.size());

    std::vector<char const *> ptrs;
    ptrs.reserve(arr.size());
    ptrs.resize(arr.size());

    for (int i = 0; i < n; ++i)
      ptrs[i] = arr[i].c_str();

    st = msg.Set(sig.c_str(), n, &ptrs[0]);
    msg.Stabilize();
  }
  else
  {
    st = msg.Set(sig.c_str(), 1, "");
    msg.Stabilize();
  }

  return st;
}

#if 0
QStatus
bridge::AllJoynHelper::SetMsgArgFromAdapterObject(adapter::NamedValue const& adapterValue, ajn::MsgArg&)
{
  // TODO:
  QStatus st = ER_OK;
  adapter::Value const& val = adapterValue.GetValue();
  QCC_UNUSED(val);
  // std::string path = deviceMain->GetBusObjectPath(adapterValue);
  return st;
}
#endif

QStatus
bridge::AllJoynHelper::MsgArgToValue(ajn::MsgArg const& m, adapter::Value& v)
{
  QStatus st = ER_OK;
  std::string const sig = ajn::MsgArg::Signature(&m, 1).c_str();

  // could be no args
  if (sig.empty()) {
    v = adapter::Value();
    return ER_OK;
  }

  auto itr = s_signatureToType.find(sig);
  DSB_ASSERT(itr != s_signatureToType.end());

  switch (itr->second)
  {
    case adapter::TypeId::Null:    st = ER_OK; break;
    case adapter::TypeId::Boolean: st = Extract<bool>(m, sig, v); break;
    case adapter::TypeId::UInt8:   st = Extract<uint8_t>(m, sig, v); break;
    case adapter::TypeId::Int16:   st = Extract<int16_t>(m, sig, v); break;
    case adapter::TypeId::UInt16:  st = Extract<uint16_t>(m, sig, v); break;
    case adapter::TypeId::Int32:   st = Extract<int32_t>(m, sig, v); break;
    case adapter::TypeId::UInt32:  st = Extract<uint32_t>(m, sig, v); break;
    case adapter::TypeId::Int64:   st = Extract<int64_t>(m, sig, v); break;
    case adapter::TypeId::UInt64:  st = Extract<uint64_t>(m, sig, v); break;
    case adapter::TypeId::Double:  st = Extract<double>(m, sig, v); break;
    case adapter::TypeId::String:  st = Extract<char *>(m, sig, v); break;

    // TODO: arrays

    default:
      assert(false);
      st = ER_NOT_IMPLEMENTED;
  }

  return st;
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

QStatus
bridge::AllJoynHelper::ValueToMsgArg(adapter::Value const& val, ajn::MsgArg& m)
{
  QStatus st = ER_OK;
  
  m.Clear();
  // empty is valid
  if (val.IsEmpty()) {
    return st;
  }
  std::string const sig = GetSignature(val.GetType());

  switch (val.GetType())
  {
    case adapter::TypeId::Null:
    break;

    case adapter::TypeId::Boolean:
    {
      bool b = val.ToBoolean();
      st = m.Set(sig.c_str(), b);
    }
    break;

    case adapter::TypeId::UInt8:
    {
      uint8_t i = val.ToUInt8();
      st = m.Set(sig.c_str(), i);
    }
    break;

    case adapter::TypeId::Int16:
    {
      int16_t i = val.ToInt16();
      st = m.Set(sig.c_str(), i);
    }
    break;

    case adapter::TypeId::UInt16:
    {
      uint16_t u = val.ToUInt16();
      st = m.Set(sig.c_str(), u);
    }
    break;

    case adapter::TypeId::Int32:
    {
      int32_t i = val.ToInt32();
      st = m.Set(sig.c_str(), i);
    }
    break;

    case adapter::TypeId::UInt32:
    {
      uint32_t u = val.ToUInt32();
      st = m.Set(sig.c_str(), u);
    }
    break;

    case adapter::TypeId::Int64:
    {
      int64_t l = val.ToInt64();
      st = m.Set(sig.c_str(), l);
    }
    break;

    case adapter::TypeId::UInt64:
    {
      uint64_t l = val.ToUInt64();
      st = m.Set(sig.c_str(), l);
    }
    break;

    case adapter::TypeId::Double:
    {
      double d = val.ToDouble();
      st = m.Set(sig.c_str(), d);
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

std::string
bridge::AllJoynHelper::GetSignature(adapter::NamedValue::Vector const& v)
{
  // TODO:
  std::stringstream buff;
  for (auto arg : v)
  {
    buff << AllJoynHelper::GetSignature(arg.GetValue().GetType());
  }
  return buff.str();
}

std::string
bridge::AllJoynHelper::GetMethodArgumentNames(adapter::NamedValue::Vector const& in, adapter::NamedValue::Vector const& out)
{
  int i = 0;

  std::stringstream buff;
  for (auto arg : in)
  {
    if (i++ != 0)
      buff << ",";
    buff << arg.GetName();
  }

  for (auto arg : out)
  {
    if (i++ != 0)
      buff << ",";
    buff << arg.GetName();
  }
  return buff.str();
}
