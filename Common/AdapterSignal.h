#pragma once

#include "Common/AdapterObject.h"
#include "Common/AdapterValue.h"

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

    Value::Vector const& GetParameters() const
      { return m_params; }

    void AddParameter(Value const& value)
      { m_params.push_back(value); }

    void ClearParameters()
      { m_params.clear(); }

  private:
    void SetInterfaceName(std::string const& name)
      { m_interface = name; }

  private:
    std::string           m_interface;
    Value::Vector  m_params;
  };

  inline Signal MakeAdapterSignal(std::string const& name,
    Value::Vector const& params = Value::Vector())
  {
    Signal signal(name);
    for (auto const& param : params)
      signal.AddParameter(param);
    return std::move(signal);
  }
}
