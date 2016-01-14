#pragma once

#include "Bridge/IAdapter.h"

namespace adapters
{
namespace mock
{
  class MockAdapterDevice;

  class MockAdapterMethod : public bridge::IAdapterMethod
  {
    friend class MockAdapter;

  public:
    MockAdapterMethod(std::string const& name, std::weak_ptr<MockAdapterDevice> const& parent);

    virtual std::string GetName() const;
    virtual std::string GetDescription();

    virtual bridge::AdapterValueVector const& GetInputParams() const;
    virtual bridge::AdapterValueVector const& GetOutputParams() const;

    virtual void SetInputParams(bridge::AdapterValueVector const& params);
    virtual void SetOutputParams(bridge::AdapterValueVector const& params);

    void AddInputParam(std::shared_ptr<bridge::IAdapterValue> const& p);
    void AddOutputParam(std::shared_ptr<bridge::IAdapterValue> const& p);

    virtual int32_t GetResult();
    void SetResult(int32_t st);

  private:
    std::shared_ptr<MockAdapterDevice> GetParent() const
      { return m_parent.lock(); }

  private:
    std::string                   m_name;
    std::string                   m_description;
    bridge::AdapterValueVector    m_inputParams;
    bridge::AdapterValueVector    m_outputParams;
    std::weak_ptr<MockAdapterDevice>   m_parent;
    int32_t                       m_result;
  };
}
}

