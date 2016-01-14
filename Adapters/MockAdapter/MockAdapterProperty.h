#pragma once

#include "Bridge/IAdapter.h"

namespace adapters
{
namespace mock
{
  class MockAdapterValue;
  class MockPropertyDescriptor;
  class MockAdapterDevice;
  
  class MockAdapterProperty : public bridge::IAdapterProperty
  {
  public:
    MockAdapterProperty(MockPropertyDescriptor const& desc, std::weak_ptr<MockAdapterDevice> const& parent);

    virtual std::string GetName();
    virtual std::string GetInterfaceHint()
      { return std::string(""); }

    virtual bridge::AdapterValueVector GetAttributes();

    std::shared_ptr<MockAdapterDevice> GetParent()
      { return m_parent.lock(); }

    std::shared_ptr<MockAdapterValue> GetAttributeByName(std::string const& name);

  private:
    std::string                 m_name;
    bridge::AdapterValueVector  m_attributes;
    std::weak_ptr<MockAdapterDevice> m_parent;
  };
}
}
