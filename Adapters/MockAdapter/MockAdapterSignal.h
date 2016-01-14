#pragma once

#include "Bridge/IAdapter.h"

#include <memory>
#include <string>

namespace adapters
{
namespace mock
{
  class MockAdapterDevice;
  
  class MockAdapterSignal : public bridge::IAdapterSignal
  {
  public:
    MockAdapterSignal(
      std::string const& name,
      std::weak_ptr<MockAdapterDevice> const& parent,
      bridge::AdapterValueVector const& params);

    virtual std::string GetName() const;
    virtual bridge::AdapterValueVector const& GetParams() const;

    std::shared_ptr<MockAdapterSignal> Clone();

  private:
    std::string                     m_name;
    std::weak_ptr<MockAdapterDevice>     m_parent;
    bridge::AdapterValueVector      m_params;
  };
}
}

