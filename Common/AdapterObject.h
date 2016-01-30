#pragma once

#include <atomic>
#include <map>
#include <stdint.h>

#include "Common/Value.h"

namespace adapter
{
  typedef std::map< std::string, Value > AttributeMap;

  class Object
  {
  public:
    Object(std::string const& name)
      : m_name(name)
      , m_id(GetNextObjectId())
    {
    }

    virtual ~Object() { }

    uint64_t GetId() const
      { return m_id; }

    std::string const& GetName() const
      { return m_name; }

    std::string const& GetDescription() const
      { return m_description; }

    void SetDescription(std::string const& desc)
      { m_description = desc; }

    AttributeMap const& GetAttributes() const
      { return m_attrs; }

    Value GetAttribute(std::string const& name) const
    {
      auto itr = m_attrs.find(name);
      return itr != m_attrs.end() ? itr->second : Value();
    }

    void AddAttribute(std::string const& name, Value const& value)
      { m_attrs.insert(AttributeMap::value_type(name, value)); }

    void ClearAttributes()
      { m_attrs.clear(); }

  private:
    std::string           m_name;
    uint64_t              m_id;
    AttributeMap   m_attrs;
    std::string           m_description;

  private:
    uint64_t GetNextObjectId()
    {
      static std::atomic<uint64_t> s_id;
      return s_id++;
    }
  };
}
