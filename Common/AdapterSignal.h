#pragma once

#include "Common/AdapterObject.h"
#include "Common/AdapterNamedValue.h"

#include <string>
#include <vector>

namespace adapter
{
  class Interface;
  
  class Signal : public adapter::Object
  {
    friend class Interface;

  public:
    typedef std::vector<Signal> Vector;

    Signal(std::string const& name)
      : adapter::Object(name)
    {
    }

    std::string const& GetInterface() const
      { return m_interface; }

    NamedValue::Vector const& GetParameters() const
      { return m_params; }

    void AddParameter(NamedValue const& value)
      { m_params.push_back(value); }

    void ClearParameters()
      { m_params.clear(); }

  private:
    void SetInterfaceName(std::string const& name)
      { m_interface = name; }

  private:
    std::string           m_interface;
    NamedValue::Vector  m_params;
  };

  inline Signal MakeAdapterSignal(std::string const& name,
    NamedValue::Vector const& params = NamedValue::Vector())
  {
    Signal signal(name);
    for (auto const& param : params)
      signal.AddParameter(param);
    return std::move(signal);
  }
}
