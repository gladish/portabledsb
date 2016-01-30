#pragma once

#include "Common/Value.h"

#include <memory>
#include <vector>

namespace adapter
{
  class NamedValue
  {
  public:
    typedef std::vector<NamedValue> Vector;

    NamedValue()
    {
    }

    NamedValue(std::string const& name, Value const& value)
      : m_name(name)
      , m_value(value) { }

    std::string const& GetName() const
      { return m_name; }

    Value const& GetValue() const
      { return m_value; }

    void SetName(std::string const& name)
      { m_name = name; }

    void SetValue(Value const& value)
      { m_value = value; }

    bool IsNull() const
      { return m_name.empty() && m_value.IsEmpty(); }

    static NamedValue const& Null()
    {
      static NamedValue kNullValue("", Value());
      return kNullValue;
    }

  private:
    std::string   m_name;
    Value       m_value;
  };
}
