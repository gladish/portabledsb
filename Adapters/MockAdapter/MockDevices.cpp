#include "Adapters/MockAdapter/MockAdapter.h"

namespace
{
  void AddClusterToDevice(adapter::Device& dev, uint16_t clusterId)
  {
    std::shared_ptr<adapters::mock::InterfaceDefinition> def = adapters::mock::GetZigBeeCluster(clusterId);
    if (def)
    {
      adapter::Interface ifc(def->Name);
      ifc.AddAttribute("code", clusterId);
      for (auto const& m : def->Methods)
        ifc.AddMethod(m);
      for (auto const& p : def->Properties)
        ifc.AddProperty(p);
      for (auto const& s : def->Signals)
        ifc.AddSignal(s);
      dev.AddInterface(std::move(ifc));
    }
  }
}


adapter::Device
adapters::mock::MockAdapter::CreateDoorWindowSensor(std::string const& name)
{
  adapter::ItemInformation info;
  info.SetName(name);
  info.SetVendor("Comcast");
  info.SetModel("801417");
  info.SetVersion("1.0");
  info.SetFirmwareVersion("1.0");
  info.SetSerialNumber("H123ALK");
  info.SetDescription("Door/Window sensor");

  adapter::Device dev;
  dev.SetBasicInfo(info);

  AddClusterToDevice(dev, 0x0000); // basic
  AddClusterToDevice(dev, 0x000f); // binary input

  return std::move(dev);
}
