//
// Copyright (c) 2016, Comcast Cable Communications Management, LLC
//
// Permission to use, copy, modify, and/or distribute this software for any purpose with or
// without fee is hereby granted, provided that the above copyright notice and this
// permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
// SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
// ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.
//
#pragma once

#include "common/Guid.h"
#include "bridge/AllJoynHeaders.h"

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
    QStatus SetApplicationGuid(adapter::Guid const& guid);
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

