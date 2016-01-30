#include "Adapters/MockAdapter/MockAdapter.h"
#include "Common/TypeId.h"

#include <thread>
#include <mutex>

namespace
{
  using namespace adapter;;
  typedef std::map< uint16_t, std::shared_ptr<adapter::Interface> > InterfaceMap;

  std::once_flag flag1;
  InterfaceMap gInterfaces;

  void CreateBinaryInput(InterfaceMap& map)
  {
    std::string name("org.zigbee.general.BinaryInput");

    std::shared_ptr<adapter::Interface> def(new adapter::Interface(name));
    def->AddProperty(MakeProperty("ActiveText", TypeId::String, AccessType::ReadWrite, {
          { "code", 0x0004 },
          { "length", 16 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("Description", TypeId::String, AccessType::ReadWrite, {
          { "code", 0x001c },
          { "length", 16 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("InactiveText", TypeId::String, AccessType::ReadWrite, {
          { "code", 0x002e },
          { "length", 16 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("OutOfService", TypeId::Boolean, AccessType::ReadWrite, {
          { "code", 0x002e },
          { "optional", false }
          } ));
    def->AddProperty(MakeProperty("Polarity", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0054 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("PresentValue", TypeId::Boolean, AccessType::ReadWrite, {
          { "code", 0x0055 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("Reliability", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0067 },
          { "min", 0x00 },
          { "max", 0x0f },
          { "optional", false }
          } ));
    def->AddProperty(MakeProperty("StatusFlags", TypeId::UInt8, AccessType::Read, {
          { "code", 0x006f },
          { "min", 0x00 },
          { "max", 0x0f },
          { "optional", false }
          } ));
    def->AddProperty(MakeProperty("ApplicationType", TypeId::UInt32, AccessType::Read, {
          { "code", 0x0100 },
          { "min", 0x00000000 },
          { "max", 0xffffffff },
          { "optional", true }
          } ));
    def->AddSignal(MakeAdapterSignal("StatusFlags", {
          { "PresentValue", false },
          } ));
    map.insert(InterfaceMap::value_type(0x000f, def));
  }

  void CreateBasicCluster(InterfaceMap& map)
  {
    std::string name("org.zigbee.general.Basic");
    std::shared_ptr<adapter::Interface> def(new adapter::Interface(name));

    def->AddProperty(MakeProperty("Version", TypeId::UInt8, AccessType::Read, { 
          { "code", 0x0000 },
          { "min", 0x00 },
          { "max", 0xff },
          { "default", 0x01 },
          { "optional", false }
          } ));
    def->AddProperty(MakeProperty("ApplicationVersion", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0001 },
          { "min", 0x00 },
          { "max", 0xff },
          { "default", 0x00 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("StackVersion", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0002 },
          { "min", 0x00 },
          { "max", 0xff },
          { "default", 0x00 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("HardwareVersion", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0003 },
          { "min", 0x00 },
          { "max", 0xff },
          { "default", 0x00 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("ManufacturerName", TypeId::String, AccessType::Read, {
          { "code", 0x0004 },
          { "length", 32 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("ModelIdentifier", TypeId::String, AccessType::Read, {
          { "code", 0x0005 },
          { "length", 32 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("DateCode", TypeId::String, AccessType::Read, {
          { "code", 0x0006 },
          { "length", 16 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("PowerSource", TypeId::String, AccessType::Read, {
          { "code", 0x0007 },
          { "min", 0x00 },
          { "max", 0xff },
          { "optional", false }
          } ));
    def->AddProperty(MakeProperty("ApplicationProfileVersion", TypeId::UInt32, AccessType::Read, {
          { "code", 0x0008 },
          { "min", 0x00000000 },
          { "max", 0xffffffff },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("LocationDescription", TypeId::String, AccessType::Read, {
          { "code", 0x0010 },
          { "length", 16 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("PhysicalEnvironment", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0011 },
          { "min", 0x00 },
          { "max", 0xff },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("DeviceEnabled", TypeId::Boolean, AccessType::Read, {
          { "code", 0x0012 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("AlarmMask", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0013 },
          { "min", 0x00 },
          { "max", 0x02 },
          { "optional", true }
          } ));
    def->AddProperty(MakeProperty("DisableLocalConfig", TypeId::UInt8, AccessType::Read, {
          { "code", 0x0014 },
          { "min", 0x00 },
          { "max", 0x03 },
          { "optional", true }
          } ));

    // methods
    def->AddMethod(MakeMethod("ResetToFactoryDefaults",
          { { "code", 0x00 }, { "optional", true } },  // attributes
          "Command that resets all attribute values to factory default.", // description
          { },  // input args
          { }   // output arg
          ));

    map.insert(InterfaceMap::value_type(0x0000, def));
  }

  void LoadClusterDatabase()
  {
    // TODO: load from configuration xml or json file.
    CreateBasicCluster(gInterfaces);
    CreateBinaryInput(gInterfaces);
  }

  void LoadClusterDatabaseOnce()
  {
    std::call_once(flag1, LoadClusterDatabase);
  }
}

std::shared_ptr<adapter::Interface>
adapters::mock::GetZigBeeCluster(uint16_t clusterId)
{
  LoadClusterDatabaseOnce();

  auto e = gInterfaces.find(clusterId);
  return (e != gInterfaces.end()) 
    ? e->second
    : std::shared_ptr<adapter::Interface>();
}
