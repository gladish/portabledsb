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
#include "AllJoynAbout.h"
#include "common/AdapterLog.h"

namespace
{
  adapter::Guid const kDefaultAppGuid = adapter::Guid::Parse("DA9C7763-C3E0-4B3B-B190-5BEF8DF96E4D");
  char const* kDefaultLanguageForAbout = "en";
  char const* kUnknownAdapter = "Unknown Device";
  char const* kUnknownManufacturer = "Unknown";
  char const* kUknownVersion = "0.0.0.0";
  char const* kDefaultAppName = "Device System Bridge";
  char const* kDefaultModel = "DSB";
  char const* kDefaultDescription = "Device System Bridge";

  int const kDsbServicePort = 27;

  DSB_DECLARE_LOGNAME(AllJoynAbout);
}

using namespace bridge;

AllJoynAbout::AllJoynAbout()
  : m_isAnnounced(false)
{
}


AllJoynAbout::~AllJoynAbout()
{
  Shutdown();
}

QStatus
AllJoynAbout::Initialize(ajn::BusAttachment& bus)
{
  m_aboutObj.reset(new ajn::AboutObj(bus, ajn::BusObject::UNANNOUNCED));
  m_aboutData.reset(new ajn::AboutData(kDefaultLanguageForAbout));
  return SetDefaultAboutData();
}

QStatus
AllJoynAbout::Shutdown()
{
  if (m_isAnnounced)
  {
    m_aboutObj->Unannounce();
    m_isAnnounced = false;
  }

  if (m_aboutObj)
    m_aboutObj.reset();

  if (m_aboutData)
    m_aboutData.reset();

  return ER_OK;
}

QStatus
AllJoynAbout::SetDefaultAboutData()
{
  std::string deviceId;
  std::string deviceName;

  GetDeviceId(deviceId);
  m_aboutData->SetDeviceId(deviceName.c_str());

  SetDeviceName(kUnknownAdapter);
  SetApplicationName(kDefaultAppName);
  SetApplicationGuid(kDefaultAppGuid);
  SetManufacturer(kUnknownManufacturer);
  SetModel(kDefaultModel);
  SetVersion(kUknownVersion);
  SetDescription(kDefaultDescription); 

  if (!m_aboutData->IsValid(kDefaultLanguageForAbout))
  {
    DSBLOG_WARN("AboutData is invalid");
  }

  return ER_OK;
}

QStatus
AllJoynAbout::GetDeviceId(std::string& deviceId)
{
  deviceId.clear();

  std::string tempId;

  QStatus st = ER_OK;
  if ((st = ReadDeviceId(tempId)) == ER_OK)
    st = CreateAndSaveDeviceId(tempId);

  if (st == ER_OK)
    deviceId = tempId;

  return st;
}

QStatus
AllJoynAbout::ReadDeviceId(std::string&)
{
  // TODO read from config
  return ER_FAIL;
}

QStatus
AllJoynAbout::CreateAndSaveDeviceId(std::string& deviceId)
{
  adapter::Guid guid = adapter::Guid::NewGuid();
  deviceId = guid.ToString();

  // TODO: need to persist this configuration 
  return ER_OK;
}


QStatus
AllJoynAbout::SetManufacturer(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetManufacturer(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetDeviceName(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetDeviceName(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetVersion(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetSoftwareVersion(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetApplicationName(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetAppName(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetApplicationGuid(adapter::Guid const& guid)
{
  return m_aboutData
    ? m_aboutData->SetAppId(guid.ToString().c_str())
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetDeviceId(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetDeviceId(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetModel(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetModelNumber(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::SetDescription(char const* s)
{
  return m_aboutData
    ? m_aboutData->SetDescription(s)
    : ER_INVALID_ADDRESS;
}

QStatus
AllJoynAbout::AddObject(ajn::BusObject& obj, ajn::InterfaceDescription const& ifc)
{
  return obj.SetAnnounceFlag(&ifc, ajn::BusObject::ANNOUNCED);
}

QStatus
AllJoynAbout::RemoveObject(ajn::BusObject& obj, ajn::InterfaceDescription const& ifc)
{
  return obj.SetAnnounceFlag(&ifc, ajn::BusObject::UNANNOUNCED);
}

QStatus
AllJoynAbout::Announce()
{
  QStatus st = ER_INIT_FAILED;
  if (m_aboutObj)
    st = m_aboutObj->Announce(kDsbServicePort, *m_aboutData);

  if (st == ER_OK)
    m_isAnnounced = true;

  return st;
}
