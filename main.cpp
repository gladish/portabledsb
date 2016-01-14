
#include "Bridge/Bridge.h"
#include "Bridge/IAdapter.h"
#include "Common/Log.h"
#include "Adapters/MockAdapter/MockAdapter.h"

#include "Common/Variant.h"
#include <iostream>

namespace
{
  DSB_DECLARE_LOGNAME(Main);

  inline std::shared_ptr<bridge::DeviceSystemBridge> DSB()
  {
    std::shared_ptr<bridge::DeviceSystemBridge> b = bridge::DeviceSystemBridge::GetInstance();
    assert(b.get() != nullptr);
    return b;
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

  QStatus st = ER_OK;

  bridge::DeviceSystemBridge::InitializeSingleton(
      std::shared_ptr<bridge::IAdapter>(new adapters::mock::MockAdapter()));

  st = DSB()->Initialize();
  if (st != ER_OK)
  {
    DSBLOG_ERROR("failed to initialize bridge: %s", QCC_StatusText(st));
    return 1;
  }

  std::shared_ptr<bridge::IAdapter> adapter = DSB()->GetAdapter();

  common::Logger::SetLevel(adapter->GetAdapterName(), common::LogLevel::Debug);

  bridge::AdapterDeviceVector deviceList;
  std::shared_ptr<bridge::IAdapterIoRequest> req;

    // TODO: do we need enums for this?
  int32_t ret = adapter->EnumDevices(bridge::EnumDeviceOptions::ForceRefresh, deviceList, &req);
  if (ret != 0)
  {
    std::cout << "EnumDevices:" << ret << std::endl;
    return 1;
  }

  for (auto const& d : deviceList)
  {
    std::cout << "Device" << std::endl;
    std::cout << "\tName:" << d->GetName() << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
  }

  getchar();
  return 0;
}

