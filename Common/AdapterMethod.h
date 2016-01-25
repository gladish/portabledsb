#pragma once

#include "Common/AdapterObject.h"
#include "Common/AdapterValue.h"

#include <string>
#include <vector>

namespace adapter
{
  class Method : public adapter::Object
  {
    friend class Interface;

  public:
    typedef std::vector<Method> Vector;

    Method(std::string const& name, std::string const& description = std::string())
      : adapter::Object(name)
    {
      SetDescription(description);
    }

    std::string const& GetInterface() const
      { return m_interface; }

    Value::Vector const& GetInputArguments() const
      { return m_inArgs; }

    void CearInputArguments()
      { m_inArgs.clear(); }

    void AddInputArgument(Value const& value)
      { m_inArgs.push_back(value); }

    Value::Vector const& GetOutputArguments() const
      { return m_outArgs; }

    void ClearOutputArguments()
      { m_outArgs.clear(); }

    void AddOutputArgument(Value const& value)
      { m_outArgs.push_back(value); }

  private:
    void SetInterfaceName(std::string const& name)
      { m_interface = name; }

  private:
    std::string m_interface;
    Value::Vector m_inArgs;
    Value::Vector m_outArgs;
  };

  inline Method MakeMethod(
      std::string const& name,
      Value::Vector const& attrs = Value::Vector(),
      std::string const& description = std::string(),
      Value::Vector const& inputArgs = Value::Vector(),
      Value::Vector const& outputArgs = Value::Vector())
  {
    Method method(name);
    for (auto attr : attrs)
      method.AddAttribute(attr.GetName(), attr.GetValue());
    for (auto in : inputArgs)
      method.AddInputArgument(in);
    for (auto out : outputArgs)
      method.AddOutputArgument(out);
    method.SetDescription(description);
    return std::move(method);
  }
}
