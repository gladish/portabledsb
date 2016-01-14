#pragma once

#include "Common/Guid.h"
#include "Bridge/AllJoynHeaders.h"

#include <memory>

namespace bridge
{
  class AllJoynAbout
  {
  public:
    AllJoynAbout();
    ~AllJoynAbout();

    QStatus Initialize(ajn::BusAttachment& bus);
    QStatus Shutdown();
    QStatus Announce();
    QStatus AddObject(ajn::BusObject& obj, ajn::InterfaceDescription const& ifc);
    QStatus RemoveObject(ajn::BusObject& obj, ajn::InterfaceDescription const& ifc);

    // TODO: are these really needed? 
    QStatus SetManufacturer(char const* s);
    QStatus SetDeviceName(char const* s);
    QStatus SetVersion(char const* s);
    QStatus SetApplicationName(char const* s);
    QStatus SetApplicationGuid(common::Guid const& guid);
    QStatus SetDeviceId(char const* s);
    QStatus SetModel(char const* s);
    QStatus SetDescription(char const* s);

  private:
    QStatus SetDefaultAboutData();
    QStatus GetDeviceId(std::string& deviceId);
    QStatus CreateAndSaveDeviceId(std::string& deviecId);
    QStatus ReadDeviceId(std::string& deviceId);

  private:
    // scoped_ptr
    std::shared_ptr<ajn::AboutData>  m_aboutData;
    std::shared_ptr<ajn::AboutObj>   m_aboutObj;
    bool                        m_isAnnounced;
  };
}

