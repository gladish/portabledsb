#pragma once

#include "Bridge/IAdapter.h"

namespace adapters
{
namespace mock
{
  class MockAdapterValue : public bridge::IAdapterValue
  {
  public:
    MockAdapterValue(std::string const& name);

    virtual std::string GetName();
    virtual common::Variant const& GetData() const;
    virtual void SetData(common::Variant const& v);

  private:
    std::string                 m_name;
    common::Variant             m_data;
  };
}
}
