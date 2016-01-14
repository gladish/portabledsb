#include "Adapters/MockAdapter/MockAdapterProperty.h"
#include "Adapters/MockAdapter/MockAdapterValue.h"
#include "Adapters/MockAdapter/MockDevices.h"

using namespace adapters::mock;

MockAdapterProperty::MockAdapterProperty( MockPropertyDescriptor const& desc,
  std::weak_ptr<MockAdapterDevice> const& parent)
  : m_name(desc.Name)
  , m_parent(parent)
{
}

std::string
MockAdapterProperty::GetName()
{
  return m_name;
}

std::shared_ptr<MockAdapterValue>
MockAdapterProperty::GetAttributeByName(std::string const& name)
{
  typedef bridge::AdapterValueVector::const_iterator iterator;
  for (iterator begin = m_attributes.begin(), end = m_attributes.end(); begin != end; ++begin)
  {
    if ((*begin)->GetName() == name)
      return std::dynamic_pointer_cast<MockAdapterValue>(*begin);
  }
  return std::shared_ptr<MockAdapterValue>();
}

bridge::AdapterValueVector
MockAdapterProperty::GetAttributes()
{
  return m_attributes;
}
