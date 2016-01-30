
#include <alljoyn/Init.h>
#include <alljoyn/BusAttachment.h>

#include <vector>
#include <thread>
#include <condition_variable>

ajn::BusAttachment* bus = nullptr;
ajn::ProxyBusObject* obj = nullptr;

bool ready = false;
std::mutex mutex;
std::condition_variable cond;

class MyAboutListener : public ajn::AboutListener
{
  void Announced(const char* busName, uint16_t version, ajn::SessionPort port,
      const ajn::MsgArg& objectDescriptionArg, const ajn::MsgArg& aboutDataArg) 
  {
    ajn::AboutObjectDescription objectDescription;
    objectDescription.CreateFromMsgArg(objectDescriptionArg);

    size_t n = objectDescription.GetPaths(nullptr, 0);
    std::vector< char const * > paths;
    paths.reserve(n);
    paths.resize(n);
    objectDescription.GetPaths(&paths[0], paths.size());

    for (auto path : paths)
    {
      if (strcmp(path, "/zigbee/RearDoor") == 0)
      {
        ajn::SessionId sessionId;
        ajn::SessionOpts opts(ajn::SessionOpts::TRAFFIC_MESSAGES, false, ajn::SessionOpts::PROXIMITY_ANY, ajn::TRANSPORT_ANY);
        bus->EnableConcurrentCallbacks();
        bus->JoinSession(busName, port, NULL, sessionId, opts);

        obj = new ajn::ProxyBusObject(*bus, busName, path, sessionId);

        std::unique_lock<std::mutex> lock(mutex);
        ready = true;
        lock.unlock();
        cond.notify_one();
      }
    }
  }
};

int main(int argc, char* argv[])
{
  AllJoynInit();

  bus = new ajn::BusAttachment("TestGetSet", true);
  bus->Start();
  bus->Connect();

  MyAboutListener aboutListener;
  bus->RegisterAboutListener(aboutListener);

  bus->WhoImplements(nullptr);

  std::unique_lock<std::mutex> lock(mutex);
  cond.wait(lock, [] { return ready; } );
  lock.unlock();

  obj->IntrospectRemoteObject();

  size_t n = obj->GetInterfaces();
  std::vector<ajn::InterfaceDescription const *> interfaces;
  interfaces.reserve(n);
  interfaces.resize(n);
  obj->GetInterfaces(&interfaces[0], interfaces.size());



    // GET PROPERTY
  ajn::MsgArg value;
  QStatus st = obj->GetProperty("org.zigbee.general.BinaryInput", "Description", value);
  if (st == ER_OK)
    printf("value: %s\n", ajn::MsgArg::ToString(&value, 1).c_str());



    // SET PROPERTY
  value.Set("s", "This is a new description");
  st = obj->SetProperty("org.zigbee.general.BinaryInput", "Description", value);
  if (st != ER_OK)
    printf("failed to set value: %s\n", QCC_StatusText(st));


    // GET PROPERTY
  st = obj->GetProperty("org.zigbee.general.BinaryInput", "Description", value);
  if (st == ER_OK)
    printf("value: %s\n", ajn::MsgArg::ToString(&value, 1).c_str());

    // INVOKE METHOD (NO ARGS)
  st = obj->MethodCall("org.zigbee.general.Basic", "ResetToFactoryDefaults", nullptr, 0);
  printf("invoke method: %s\n", QCC_StatusText(st));

  return 0;
}
