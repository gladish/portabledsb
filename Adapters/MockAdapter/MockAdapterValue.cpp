#include "Adapters/MockAdapter/MockAdapterValue.h"

using namespace adapters::mock;

MockAdapterValue::MockAdapterValue(std::string const& name)
  : m_name(name)
{
}

std::string
MockAdapterValue::GetName()
{
  return m_name;
}

common::Variant const&
MockAdapterValue::GetData() const
{
  return m_data;
}

void
MockAdapterValue::SetData(common::Variant const& v)
{
  m_data = v;
}
