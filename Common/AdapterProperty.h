#pragma once

#include <atomic>
#include <string>

#include "Common/AdapterObject.h"
#include "Common/AdapterValue.h"
#include "Common/AccessType.h"
#include "Common/SignalBehavior.h"
#include "Common/TypeId.h"

namespace adapter
{
  class Interface;

  class Property : public adapter::Object
  {
    friend class Interface;

  public:
    typedef std::vector<Property> Vector;

    Property(std::string const& name, TypeId type, AccessType access) 
      : adapter::Object(name)
      , m_type(type)
      , m_access(access)
    {
    }

    std::string const& GetInterfaceName() const
      { return m_interface; }

    TypeId GetType() const
      { return m_type; }

    AccessType GetAccess() const
      { return m_access; }

    void SetAccess(AccessType access)
      { m_access = access; }

    SignalBehavior GetChangeOfValueBehavior() const
      { return m_changeOfValueBehavior; }

    void SetGetChangeOfValueBehavior(SignalBehavior changeOfValueBehavior)
      { m_changeOfValueBehavior = changeOfValueBehavior; }

  private:
    void SetInterfaceName(std::string const& name)
      { m_interface = name; }

  private:
    std::string           m_interface;
    TypeId                m_type;
    AccessType            m_access;
    SignalBehavior        m_changeOfValueBehavior;
  };

  inline Property MakeProperty(std::string const& name, TypeId type,
    AccessType access, Value::Vector const& attrs = Value::Vector())
  {
    Property prop(name, type, access);
    for (auto const& attr : attrs)
      prop.AddAttribute(attr.GetName(), attr.GetValue());
    return std::move(prop);
  }
}
