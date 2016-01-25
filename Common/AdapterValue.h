#pragma once

#include "Common/Variant.h"

#include <memory>
#include <vector>

namespace adapter
{
  class Value
  {
  public:
    typedef std::vector<Value> Vector;

    Value()
    {
    }

    Value(std::string const& name, Variant const& value)
      : m_name(name)
      , m_value(value) { }

    std::string const& GetName() const
      { return m_name; }

    Variant const& GetValue() const
      { return m_value; }

    void SetName(std::string const& name)
      { m_name = name; }

    void SetValue(Variant const& value)
      { m_value = value; }

    bool IsNull() const
      { return m_name.empty() && m_value.IsEmpty(); }

    static Value const& Null()
    {
      static Value kNullValue("", Variant());
      return kNullValue;
    }

  private:
    std::string   m_name;
    Variant       m_value;
  };
}
