#include "Adapters/MockAdapter/MockAdapterSignal.h"

using namespace adapters::mock;

MockAdapterSignal::MockAdapterSignal(
      std::string const& name,
      std::weak_ptr<MockAdapterDevice> const& parent,
      bridge::AdapterValueVector const& params)
  : m_name(name)
  , m_parent(parent)
  , m_params(params)
{
}

std::shared_ptr<MockAdapterSignal>
MockAdapterSignal::Clone()
{
  std::shared_ptr<MockAdapterSignal> clone(new MockAdapterSignal(
    m_name,
    m_parent,
    m_params));
  return clone;
}


std::string
MockAdapterSignal::GetName() const
{
  return m_name;
}

bridge::AdapterValueVector const&
MockAdapterSignal::GetParams() const
{
  return m_params;
}
