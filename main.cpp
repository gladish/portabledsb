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
#include "bridge/Bridge.h"
#include "common/Adapter.h"
#include "common/AdapterLoader.h"
#include "common/AdapterLog.h"

#include <getopt.h>

static option longOptions[] =
{
  { "adapter", required_argument, 0, 'a' },
  { "config",  required_argument, 0, 'c' },
  { 0, 0, 0, 0 }
};

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

int main(int argc, char* argv[])
{
  #if 0
  std::cout << "size adapter::Device : " << sizeof(adapter::Device) << std::endl;
  std::cout << "size adapter::Method : " << sizeof(adapter::Method) << std::endl;
  std::cout << "size adapter::Prop   : " << sizeof(adapter::Property) << std::endl;
  std::cout << "size adpater::Intf   : " << sizeof(adapter::Interface) << std::endl;
  std::cout << "size std::string     : " << sizeof(std::string) << std::endl;
  std::cout << "size adapter::Guid   : " << sizeof(adapter::Guid) << std::endl;
  std::cout << "size string map<>    : " << sizeof(std::map<std::string, std::string>) << std::endl;
  #endif

  int optionIndex = 0;
  char const* adapterName = nullptr;
  char const* configFile = "alljoyndsb.xml";

  while (true)
  {
    int c = getopt_long(argc, argv, "a:c:", longOptions, &optionIndex);
    if (c == -1)
      break;

    switch (c)
    {
      case 'a':
        adapterName = optarg;
        break;
      case 'c':
        configFile = optarg;
        break;
      default:
        return 0;
    }
  }

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

  if (!adapterName)
  {
    DSBLOG_ERROR("need --adapter=<adaptername> argument");
    exit(1);
  }

  adapter::Loader loader(adapterName);
  if (!loader.bind())
    exit(1);

  std::shared_ptr<adapter::Adapter> adapter = loader.create();
  bridge::Bridge::InitializeSingleton(adapter);

  try
  {
    DSB()->Initialize(configFile);
  }
  catch (std::exception const& err)
  {
    DSBLOG_ERROR("failed to initialize dsb. %s", err.what());
    exit(1);
  }


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
