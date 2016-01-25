#pragma once

#include "Common/AdapterMethod.h"
#include "Common/AdapterObject.h"
#include "Common/AdapterProperty.h"
#include "Common/AdapterSignal.h"

#include <algorithm>
#include <vector>

namespace adapter
{
  class Interface : public adapter::Object
  {
  public:
    typedef std::vector<Interface> Vector;

    Interface(std::string const& name)
      : adapter::Object(name)
    {
    }

    Property::Vector const& GetProperties() const
      { return m_props; }

    Property const* GetProperty(char const* name) const
    {
      Property const* prop = nullptr;
      
      auto itr = std::find_if(m_props.begin(), m_props.end(),
        [&name](Property const& p) { return strcmp(p.GetName().c_str(), name) == 0; });

      if (itr != m_props.end())
        prop = &(*itr);

      return prop;
    }

    void ClearProperties()
      { m_props.clear(); }

    void AddProperty(Property const& prop)
    {
      Property myProp = prop;
      myProp.SetInterfaceName(GetName());
      m_props.push_back(std::move(myProp));
    }

    Method::Vector const& GetMethods() const
      { return m_methods; }

    void ClearMethods()
      { m_methods.clear(); }

    void AddMethod(Method const& method)
    {
      Method myMethod = method;
      myMethod.SetInterfaceName(GetName());
      m_methods.push_back(std::move(myMethod));
    }

    Signal::Vector const& GetSignals() const
      { return m_signals; }

    void ClearSignals()
      { m_signals.clear(); }

    void AddSignal(Signal const& signal)
    {
      Signal mySignal = signal;
      mySignal.SetInterfaceName(GetName());
      m_signals.push_back(std::move(mySignal));
    }

  private:
    Property::Vector m_props;
    Method::Vector   m_methods;
    Signal::Vector   m_signals;
  };
}
