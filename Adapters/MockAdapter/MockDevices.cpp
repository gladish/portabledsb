#include "Adapters/MockAdapter/MockDevices.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace
{
  using namespace adapters::mock;

  const MockDeviceDescriptor kMockDevices[] = 
  {
     // Device #1 - A light bulb
    {
      1,                        // Device ID
      "Mock BACnet Switch",    // Device name
      "Microsoft",             // Vendor
      "2 X Switch",            // Model
      "001-001-001",           // Serial
      "1.1.1.1",               // Version
      "2 Gang Switch Pack",    // Description

      // Properties
      {
        // Property  #1
        {
          1,                          // Property ID
          MockPropertyType::BinaryOutput,       // Property type
          "Switch 1",                // Name
          MockPropertyAccess::ReadOnly,        // Access
          MockPropertyUnits::NoUnits,             // Units

          // Current value
          {
            ajn::ALLJOYN_BOOLEAN,  // ValueType
            "Room 1 Back Light",   // Name
            {                       // Initial value
              0,    // SimpleType
              {},     // Array of bytes
              {""}   // String
            }
          }
        },

        // Property  #2
        {
          2,                          // Property ID
          MockPropertyType::BinaryOutput,       // Property type
          "Switch 2",                // Name
          MockPropertyAccess::ReadWrite,        // Access
          MockPropertyUnits::NoUnits,             // Units

          // Current value
          {
            ajn::ALLJOYN_BOOLEAN,  // ValueType
            "Room 1 Main Light",   // Name
            {                       // Initial value
              0,    // SimpleType
              {},     // Array of bytes
              {""}   // String
            },
          }
        },

        { kLastDescriptorId } // Last property

      } // Properties[]
    },

    // Device #2 - A dimmable light bulb
    {
      2,                                  // Device ID
      "Mock BACnet Dimmable Switch",     // Device name
      "Microsoft",                       // Vendor
      "Dim Control 725",                 // Model
      "001-002-001",                     // Serial
      "1.1.2.1",                         // Version
      "Room2, Dimmable Light Switch",    // Description

      // Properties
      {
        // Property  #1
        {
          1,                          // Property ID
          MockPropertyType::BinaryOutput,       // Property type
          "Switch",                  // Name
          MockPropertyAccess::ReadWrite,        // Access
          MockPropertyUnits::NoUnits,             // Units

          // Current value
          {
            ajn::ALLJOYN_BOOLEAN,  // ValueType
            "Living Room Light",   // Name
            {                       // Initial value
              0,    // SimpleType
              {},     // Array of bytes
              {""}   // String
            },
          }
        },

        // Property  #2
        {
          2,                          // Property ID
          MockPropertyType::AnalogOutput,       // Property type
          "Dim Control",             // Name
          MockPropertyAccess::ReadWrite,        // Access
          MockPropertyUnits::Percent,              // Units

          // Current Value
          {
            ajn::ALLJOYN_UINT32,   // ValueType
            "Living Room Dimmer",  // Name
            {                       // Initial value
              50,   // SimpleType
              {},     // Array of bytes
              {""}   // WCHAR string
            },
          }
        },

        { kLastDescriptorId } // Last property

      } // Properties[]
    },



    // Device #3 - A temperature sensor
    {
      3,                                  // Device ID
      "Mock BACnet Temperature Sensor",  // Device name
      "Microsoft",                       // Vendor
      "Temperature Sensor 155",          // Model
      "001-003-001",                     // Serial
      "1.1.3.1",                         // Version
      "Temperature Sensor",              // Description

      // Properties
      {
        // Property  #1
        {
          1,                          // Property ID
          MockPropertyType::AnalogInput,        // Property type
          "Temperature",             // Name
          MockPropertyAccess::ReadOnly,        // Access
          MockPropertyUnits::DegressFahrenheit,   // Units

          // Current Value
          {
            ajn::ALLJOYN_DOUBLE,       // ValueType
            "Living Room Temperature", // Name
            {                           // Initial value
              62.5, // SimpleType
              {},     // Array of bytes
              {""}   // WCHAR string
            },
          }
        },

        { kLastDescriptorId } // Last endpoint
      }
    },
  };
}

std::vector<MockDeviceDescriptor>
adapters::mock::GetMockDevices()
{
  const int n = sizeof(kMockDevices) / sizeof(MockDeviceDescriptor);
  return std::vector<MockDeviceDescriptor>(kMockDevices, kMockDevices + n);
}

std::string
adapters::mock::PropertyUnitsToString(MockPropertyUnits units)
{
  std::string s = "Unsupported";
  switch (units)
  {
    case MockPropertyUnits::NoUnits:
    s = "NoUnits";
    break;
    
    case MockPropertyUnits::Percent:
    s = "Percent";
    break;

    case MockPropertyUnits::DegressFahrenheit:
    s = "DegressFahrenheit";
    break;
  }
  return s;
}

std::string
adapters::mock::AccessToString(MockPropertyAccess access)
{
  std::string s = "Unsupported";
  switch (access)
  {
    case MockPropertyAccess::Invalid:
    s = "Invalid Access Code";
    break;

    case MockPropertyAccess::ReadOnly:
    s = "Read Only";
    break;

    case MockPropertyAccess::ReadWrite:
    s = "Read Write";
    break;
  }
  return s;
}

std::string
adapters::mock::PropertyTypeToString(MockPropertyType type)
{
  std::string s = "Unsupported";
  switch (type)
  {
    case MockPropertyType::AnalogInput:
    s = "Analog Input";
    break;

    case MockPropertyType::AnalogOutput:
    s = "Analog Output";
    break;

    case MockPropertyType::AnalogValue:
    s = "Analog Value";
    break;

    case MockPropertyType::BinaryInput:
    s = "Binary Input";
    break;

    case MockPropertyType::BinaryOutput:
    s = "Binary Output";
    break;

    case MockPropertyType::BinaryValue:
    s = "Binary Value";
    break;
  }
  return s;
}

std::string
adapters::mock::PropertyAttributeIdToString(MockPropertyAttributeId attrId)
{
  std::string s = "Unsupported";
  switch (attrId)
  {
    case MockPropertyAttributeId::Name:
    s = "Object_Name";
    break;

    case MockPropertyAttributeId::Type:
    s = "Oject_Type";
    break;

    case MockPropertyAttributeId::Id:
    s = "Object_Identifier";
    break;

    case MockPropertyAttributeId::Access:
    s = "Access";
    break;

    case MockPropertyAttributeId::Units:
    s = "Units";
    break;

    case MockPropertyAttributeId::PresentValue:
    s = "Present Value";
    break;
  }
  return s;
}

#pragma GCC diagnostic pop
