#pragma once

#include "Common/Guid.h"
#include "Common/Variant.h"

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace bridge
{
  typedef uint32_t AdapterStatus;

  inline bool IsOk(AdapterStatus st)
  {
    return st == 0;
  }

  std::string const kDeviceArrivalSignal = "Device_Arrival";
  std::string const kDeviceArravalHandle = "Device_Handle";
  std::string const kDeviceRemovalSignal = "Device_Removal";
  std::string const kDeviceRemovalDeviceHandle = "Device_Handle";
  std::string const kChangeOfValueSignal = "Change_Of_Value";
  std::string const kCovPropertyHandle = "Property_Handle";
  std::string const kCovAttributeHandle = "Attribute_Handle";

  std::string const kDsbDeviceNotificationSignal = "DeviceNotificationSignal";
  std::string const kDsbSendMsgToDeviceMethod = "SendMessageToDeviceSynchronous";

  class IAdapterValue;
  class IAdapterProperty;
  class IAdapterMethod;
  class IAdapterSignal;
  class IAdapterDevice;
  class IAdapterLog;

  typedef std::vector< std::shared_ptr<IAdapterValue> >    AdapterValueVector;
  typedef std::vector< std::shared_ptr<IAdapterProperty> > AdapterPropertyVector;
  typedef std::vector< std::shared_ptr<IAdapterMethod> >   AdapterMethodVector;
  typedef std::vector< std::shared_ptr<IAdapterSignal> >   AdapterSignalVector;
  typedef std::vector< std::shared_ptr<IAdapterDevice> >   AdapterDeviceVector;
  typedef std::map< std::string, std::string>         AnnotationMap;

  enum class AccessType
  {
    Read,
    Write,
    ReadWrite
  };

  enum class SignalBehavior
  {
    Unspecified,
    Never,
    Always,
    AlwaysWithNoValue
  };

  enum class AdapterLogLevel
  {
    Off = -1,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Fatal = 5
  };

  class IAdapterLog
  {
  public:
    virtual void Write(AdapterLogLevel level, char const* file, int line, char const* format, ...) = 0;
    virtual bool IsLevelEnabled(AdapterLogLevel level) = 0;
  };

  class IAdapterAttribute
  {
  public:
    virtual std::shared_ptr<IAdapterValue> GetValue() = 0;
    virtual AnnotationMap GetAnnotations() = 0;
    virtual AccessType GetAccess() = 0;
    virtual SignalBehavior GetChangeOfValueBehavior() = 0;
  };

  class IAdapterValue
  {
  public:
    virtual ~IAdapterValue() { }
    virtual std::string GetName() = 0;
    virtual common::Variant const& GetData() const = 0;
    virtual void SetData(common::Variant const& msg) = 0;
  };

  class IAdapterProperty
  {
  public:
    virtual ~IAdapterProperty() { }
    virtual std::string GetName() = 0;
    virtual std::string GetInterfaceHint() = 0;
    virtual AdapterValueVector GetAttributes() = 0;
  };

  class IAdapterMethod
  {
  public:
    virtual ~IAdapterMethod() { }
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() = 0;

    virtual AdapterValueVector const& GetInputParams() const = 0;
    virtual AdapterValueVector const& GetOutputParams() const = 0;

    virtual void SetInputParams(AdapterValueVector const& params) = 0;
    virtual void SetOutputParams(AdapterValueVector const& params) = 0;

    virtual int32_t GetResult() = 0;
  };

  class IAdapterSignal
  {
  public:
    virtual ~IAdapterSignal() { }
    virtual std::string GetName() const = 0;
    virtual AdapterValueVector const& GetParams() const = 0;
  };

  class IAdapterSignalListener
  {
  public:
    virtual ~IAdapterSignalListener() { }
    virtual void AdapterSignalHandler(IAdapterSignal const& signal, void* argp) = 0;
  };

  class IAdapterIcon
  {
  public:
    virtual std::vector<uint8_t> GetImage() = 0;
    virtual std::string GetMimeType() = 0;
    virtual std::string GetUrl() = 0;
  };

  class IAdapterDevice
  {
  public:
    virtual ~IAdapterDevice() { }
    virtual std::string GetName() = 0;
    virtual std::string GetVendor() = 0;
    virtual std::string GetModel() = 0;
    virtual std::string GetVersion() = 0;
    virtual std::string GetFirmwareVersion() = 0;
    virtual std::string GetSerialNumber() = 0;
    virtual std::string GetDescription() = 0;
    virtual std::shared_ptr<IAdapterIcon> GetIcon() = 0;

    virtual AdapterPropertyVector const& GetProperties() const = 0;
    virtual AdapterMethodVector const& GetMethods() const = 0;
    virtual AdapterSignalVector const& GetSignals() const = 0;
  };

  class IAdapterIoRequest
  {
  public:
    virtual ~IAdapterIoRequest() { }

    virtual AdapterStatus GetStatus() = 0;;
    virtual AdapterStatus Wait(uint32_t timeoutMillis = std::numeric_limits<uint32_t>::max()) = 0;
    virtual AdapterStatus Cancel() = 0;
  };

  enum class EnumDeviceOptions
  {
    CacheOnly = 0,
    ForceRefresh = 1
  };

  class IAdapter
  {
  protected:
    IAdapter() { }
  public:

    typedef int32_t RegistrationHandle;

    virtual ~IAdapter() { }
    virtual std::string GetVendor() = 0;
    virtual std::string GetAdapterName() = 0;
    virtual std::string GetVersion() = 0;
    virtual std::string GetExposedAdapterPrefix() = 0;
    virtual std::string GetExposedApplicationName() = 0;
    virtual common::Guid GetExposedApplicationGuid() = 0;
    virtual AdapterSignalVector GetSignals() = 0;

    virtual std::string GetStatusText(AdapterStatus st) = 0;

    virtual AdapterStatus SetConfiguration(std::vector<uint8_t> const& configData) = 0;
    virtual AdapterStatus GetConfiguration(std::vector<uint8_t>* configData) = 0;

    virtual AdapterStatus Initialize(std::shared_ptr<IAdapterLog> const& log) = 0;
    virtual AdapterStatus Shutdown() = 0;

    virtual AdapterStatus EnumDevices(
      EnumDeviceOptions opts,
      AdapterDeviceVector& deviceList,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus GetProperty(
      std::shared_ptr<IAdapterProperty>& prop,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus SetProperty(
      std::shared_ptr<IAdapterProperty> const& prop,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus GetPropertyValue(
      std::shared_ptr<IAdapterProperty> const& prop,
      std::string const& attributeName,
      std::shared_ptr<IAdapterValue>& value,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus SetPropertyValue(
      std::shared_ptr<IAdapterProperty> const& prop,
      std::shared_ptr<IAdapterValue> const& value,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus CallMethod(
      std::shared_ptr<IAdapterMethod>& method,
      std::shared_ptr<IAdapterIoRequest>* req) = 0;

    virtual AdapterStatus RegisterSignalListener(
      std::string const& signalName,
      std::shared_ptr<IAdapterSignalListener> const& listener,
      void* argp, 
      RegistrationHandle& handle) = 0;

    virtual AdapterStatus UnregisterSignalListener(RegistrationHandle const& h) = 0;
  };


  // concrete implementations of some of the simple interfaces

  class AdapterAttribute : public IAdapterAttribute
  {
  public:
    AdapterAttribute(std::shared_ptr<IAdapterValue> const& value, AnnotationMap const& annotations,
      AccessType accessType, SignalBehavior signalBehavior)
        : m_adapterValue(value)
        , m_annotationMap(annotations)
        , m_accessType(accessType)
        , m_signalBehavior(signalBehavior)
    {
      // empty
    }

  public:
    virtual std::shared_ptr<IAdapterValue> GetValue()
      { return m_adapterValue; }

    virtual AnnotationMap GetAnnotations()
      { return m_annotationMap; }

    virtual AccessType GetAccess()
      { return m_accessType; }

    virtual SignalBehavior GetChangeOfValueBehavior()
      { return m_signalBehavior; }

  private:
    std::shared_ptr<IAdapterValue> m_adapterValue;
    AnnotationMap             m_annotationMap;
    AccessType                m_accessType;
    SignalBehavior            m_signalBehavior;
  };

  class AdapterValue : public IAdapterValue
  {
  public:
    AdapterValue(std::string const& name, common::Variant const& data)
      : m_name(name)
      , m_data(data)
    {
      // empty
    }

    virtual std::string GetName()
      { return m_name; }

    virtual common::Variant const& GetData() const
      { return m_data; }

  private:
    std::string       m_name;
    common::Variant   m_data;
  };

  class AdapterIoRequest : public IAdapterIoRequest
  {
  public:
    AdapterIoRequest()
      : m_status(0)
      , m_canceled(false)
      , m_completed(false)
    {
    }

    virtual ~AdapterIoRequest()
    {
    }

    virtual AdapterStatus GetStatus()
      { return m_status; }

    virtual AdapterStatus Wait(uint32_t millis = std::numeric_limits<uint32_t>::max())
    {
      if (millis != std::numeric_limits<uint32_t>::max())
      {
        auto delay = std::chrono::system_clock::now() + std::chrono::milliseconds(millis);

        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_cond.wait_until(lk, delay, [this] { return this->m_canceled || this->m_completed; }))
          return 0;
        else
          return -1; // timedout
      }
      else
      {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_cond.wait(lk, [this] { return this->m_canceled || this->m_completed; });
      }

      return 0;
    }

    virtual AdapterStatus Cancel()
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_canceled = true;
      m_completed = true;
      lk.unlock();
      m_cond.notify_all();

      return 0;
    }

    void SetComplete(AdapterStatus status)
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_status = status;
      m_canceled = false;
      m_completed = true;
      lk.unlock();
      m_cond.notify_all();
    }

  private:
    AdapterStatus             m_status;
    bool                      m_canceled;
    bool                      m_completed;
    std::mutex                m_mutex;
    std::condition_variable   m_cond;
  };
}

