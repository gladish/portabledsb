#include "Adapters/MockAdapter/MockAdapterMethod.h"

using namespace adapters::mock;

MockAdapterMethod::MockAdapterMethod(
      std::string const& name,
      std::weak_ptr<MockAdapterDevice> const& parent)
  : m_name(name)
  , m_parent(parent)
{
}

std::string
MockAdapterMethod::GetName() const
{
  return m_name;
}

std::string
MockAdapterMethod::GetDescription()
{
  return m_description;
}

bridge::AdapterValueVector const&
MockAdapterMethod::GetInputParams() const
{
  return m_inputParams;
}

bridge::AdapterValueVector const&
MockAdapterMethod::GetOutputParams() const
{
  return m_outputParams;
}

void
MockAdapterMethod::SetInputParams(bridge::AdapterValueVector const& params)
{
  m_inputParams = params;
}

void
MockAdapterMethod::SetOutputParams(bridge::AdapterValueVector const& params)
{
  m_outputParams = params;
}

void
MockAdapterMethod::AddInputParam(std::shared_ptr<bridge::IAdapterValue> const& p)
{
  m_inputParams.push_back(p);
}

void
MockAdapterMethod::AddOutputParam(std::shared_ptr<bridge::IAdapterValue> const& p)
{
  m_outputParams.push_back(p);
}

int32_t
MockAdapterMethod::GetResult()
{
  return m_result;
}

void
MockAdapterMethod::SetResult(int32_t st)
{
  m_result = st;
}
