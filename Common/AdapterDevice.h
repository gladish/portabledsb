#pragma once

#include "Common/AdapterItemInformation.h"
#include "Common/AdapterInterface.h"

namespace adapter
{
  class Device
  {
  public:
    typedef std::vector<Device> Vector;

    ItemInformation const& GetInfo() const
      { return m_info; }

    void SetBasicInfo(ItemInformation const& info)
      { m_info = info; }

    Interface::Vector const& GetInterfaces() const
      { return m_interfaces; }

    void AddInterface(Interface const& interface)
      { m_interfaces.push_back(interface); }

    void ClearInterfaces()
      { m_interfaces.clear(); }

  private:
    ItemInformation          m_info;
    Interface::Vector        m_interfaces;
  };
}
