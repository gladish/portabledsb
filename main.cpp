
#include "Bridge/Bridge.h"

#include "Common/Adapter.h"
#include "Common/AdapterLog.h"

#include "Adapters/MockAdapter/MockAdapter.h"

#include <iostream>

namespace
{
  struct ObjectId
  {
    uint64_t DeviceId;
    uint64_t InterfaceId;
    uint64_t MemberId;

    bool operator < (ObjectId const& rhs) const
    {
      if (DeviceId == rhs.DeviceId)
        if (InterfaceId == rhs.InterfaceId)
          return MemberId < rhs.MemberId;
        return InterfaceId < rhs.InterfaceId;
      return DeviceId < rhs.DeviceId;
    }
  };

  DSB_DECLARE_LOGNAME(Main);

  inline std::shared_ptr<bridge::Bridge> DSB()
  {
    std::shared_ptr<bridge::Bridge> b = bridge::Bridge::GetInstance();
    assert(b.get() != nullptr);
    return b;
  }

  void PrintValue(adapter::Interface const& ifc, adapter::NamedValue const& val)
  {
    std::cout << ifc.GetName() << "." << val.GetName();
    std::cout << " = " << val.GetValue().ToString();
    std::cout << " [" << adapter::TypeIdToString(val.GetValue().GetType()) << "]" << std::endl;
  }

  void PrintDevice(adapter::Adapter& adapter, adapter::Device const& dev)
  {
    std::cout << "Device [" << dev.GetInfo().GetName() << "]" << std::endl;
    for (auto ifc : dev.GetInterfaces())
    {
      std::cout << "InterfaceName:" << ifc.GetName() << std::endl;

      for (auto attr : ifc.GetAttributes())
        std::cout << "  [" << attr.first << "=" << attr.second.ToString() << "]" << std::endl;
      std::cout << std::endl;
     
      std::cout << "Properties" << std::endl;
      for (auto prop : ifc.GetProperties())
      {
        std::shared_ptr<adapter::IoRequest> req(new adapter::IoRequest());

        adapter::Value value;
        adapter.GetProperty(dev, ifc, prop, value, req);
        req->Wait();

        std::cout << " ";
        PrintValue(ifc, adapter::NamedValue(prop.GetName().c_str(), value));

        for (auto attr : prop.GetAttributes())
          std::cout << "    [" << attr.first << "=" << attr.second.ToString() << "]" << std::endl;
      }
      std::cout << std::endl;

      std::cout << "Methods" << std::endl;
      for (auto method : ifc.GetMethods())
      {
        std::cout << "  " << method.GetName() << std::endl;
        for (auto attr : method.GetAttributes())
          std::cout << "    [" << attr.first << "=" << attr.second.ToString() << "]" << std::endl;
      }
      std::cout << std::endl;
      std::cout << std::endl;
    }
  }
}

int main(int /*argc*/, char* /*argv*/ [])
{
  #if 0
  bridge::bridgeConfig config;
  config.FromFile("/some/file.xml");

  std::cout << config.GetBridgeKeyX() << std::endl;
  std::cout << config.GetDeviceKeyX() << std::endl;
  std::cout << config.GetDeviceUsername() << std::endl;
  std::cout << config.GetDevicePassword() << std::endl;
  std::cout << config.GetDeviceEcdheEcdsaPrivateKey() << std::endl;
  std::cout << config.GetDeviceEcdheEcdsaCertChain() << std::endl;
  std::cout << config.GetDefaultVisibility() << std::endl;

  bridge::DsbObjectConfig obj;
  QStatus st = config.FindObject("/my/bus/object", obj);

  obj.SetId("/my/bus/object2");
  obj.SetDescription("This is bus object2");
  obj.SetIsVisible(true);
  config.AddObject(obj);

  obj.SetId("/my/bus/object3");
  obj.SetDescription("This is bus object3");
  obj.SetIsVisible(true);
  config.AddObject(obj);
  config.ToFile("/tmp/test.xml");
  config.FromFile("/tmp/test.xml");
  config.RemoveObject("/my/bus/object2");
  config.ToFile("/tmp/test2.xml");
  #endif
  adapter::Log::SetDefaultLevel(adapter::LogLevel::Debug);

  bridge::Bridge::InitializeSingleton(std::shared_ptr<adapter::Adapter>(new adapters::mock::MockAdapter()));

  DSB()->Initialize();


  #if 0
  std::shared_ptr<adapter::Adapter> adapter = DSB()->GetAdapter();
  adapter::ItemInformation const& info = adapter->GetInfo();

  adapter::Device::Vector devices;
  std::shared_ptr<adapter::IoRequest> req(new adapter::IoRequest());
  adapter->EnumDevices(adapter::EnumDeviceOptions::ForceRefresh, devices, req);
  if (req)
  {
    DSBLOG_INFO("waiting for EnumDevices to complete");
    if (!req->Wait(20000))
      DSBLOG_INFO("EnumDevices timed out");
    else
      DSBLOG_INFO("EnumDevices completed");
  }

  for (auto const& device : devices)
    PrintDevice(*adapter, device);
  #endif

  getchar();

  bridge::Bridge::ReleaseSingleton();
  return 0;
}
