#pragma once

#include "Common/defines.h"
#include "Common/Guid.h"

#include <alljoyn/AboutData.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/Status.h>

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
    // unique_ptr
    shared_ptr<ajn::AboutData>  m_aboutData;
    shared_ptr<ajn::AboutObj>   m_aboutObj;
    bool                        m_isAnnounced;
  };
}

