#pragma once

#include <string>
#include "Common/Guid.h"

namespace adapter
{
  class ItemInformation
  {
  public:
    ItemInformation()
      : m_id(Guid::Null())
    {
    }

    std::string const& GetName() const
      { return m_name; }

    std::string const& GetVendor() const
      { return m_name; }

    std::string const& GetModel() const
      { return m_model; }

    std::string const& GetVersion() const
      { return m_version; }

    std::string const& GetFirmwareVersion() const
      { return m_firmwareVersion; }

    std::string const& GetSerialNumber() const
      { return m_serialNumber; }

    std::string const& GetDescription() const
      { return m_description; }

    Guid GetId() const
      { return m_id; }

    void SetId(Guid const& id)
      { m_id = id; }

    void SetName(std::string const& name)
      { m_name = name; }

    void SetVendor(std::string const& vendor)
      { m_vendor = vendor; }

    void SetModel(std::string const& model)
      { m_model = model; }

    void SetVersion(std::string const& version)
      { m_version = version; }

    void SetFirmwareVersion(std::string const& firmwareVersion)
      { m_firmwareVersion = firmwareVersion; }

    void SetSerialNumber(std::string const& serialNumber)
      { m_serialNumber = serialNumber; }

    void SetDescription(std::string const& description)
      { m_description = description; }

  private:
    std::string m_name;
    std::string m_vendor;
    std::string m_model;
    std::string m_version;
    std::string m_firmwareVersion;
    std::string m_serialNumber;
    std::string m_description;
    Guid        m_id;
  };
}

