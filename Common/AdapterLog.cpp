
#include "Common/AdapterLog.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#include <mutex>

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include <map>

namespace
{
  adapter::LogLevel const kDefaultLoggingLevel = adapter::LogLevel::Info;
  typedef std::map< std::string, std::shared_ptr<adapter::Log> > LogMap;

  struct LogImpl
  {
    LogImpl()
      : DefaultLevel(kDefaultLoggingLevel)
      , OutFile(stdout) { }

    adapter::LogLevel  DefaultLevel;
    std::mutex        Mutex;
    FILE*             OutFile;
    LogMap            Logs;
  };

  LogImpl impl;

  char const* LevelToString(adapter::LogLevel l)
  {
    char const* s = "unknown";
    switch (l)
    {
      case adapter::LogLevel::Off: s = "OFF"; break;
      case adapter::LogLevel::Debug: s = "DEBUG"; break;
      case adapter::LogLevel::Info: s = "INFO"; break;
      case adapter::LogLevel::Warn: s = "WARN"; break;
      case adapter::LogLevel::Error: s = "ERROR"; break;
      case adapter::LogLevel::Fatal: s = "FATAL"; break;
      default:
        break;
    }
    return s;
  }

  #if defined(__APPLE__)
  #define ThreadId_FMT "%p"
  pthread_t GetCurrentThreadId() { return pthread_self(); }
  #elif defined(__linux__)
  #define ThreadId_FMT "%ld"
  long GetCurrentThreadId() { return syscall(__NR_gettid); }
  #else
  #error ThreadId_FMT and GetCurrentThreadId() are not defined for your platform
  #endif
}

adapter::Log::Log(std::string const& name)
  : m_name(name)
{
}

void
adapter::Log::SetDefaultLevel(LogLevel level)
{
  std::lock_guard<std::mutex> lock(impl.Mutex);
  impl.DefaultLevel = level;
}

void
adapter::Log::Write(LogLevel level, char const* file, int line, char const* format, ...)
{ 
  va_list args;
  va_start(args, format);
  VaLog(level, file, line, format, args);
  va_end(args);
}

void
adapter::Log::SetLevel(LogLevel level)
{
  std::lock_guard<std::mutex> lock(impl.Mutex);
  m_level = level;
}

bool
adapter::Log::IsLevelEnabled(LogLevel level)
{
  std::lock_guard<std::mutex> lock(impl.Mutex);

  adapter::LogLevel minLevel = impl.DefaultLevel;
  LogMap::const_iterator itr = impl.Logs.find(m_name);
  if (itr != impl.Logs.end())
    minLevel = itr->second->m_level;
  return level >= minLevel;
}

void
adapter::Log::VaLog(LogLevel level, const char* file, int line, const char* format, va_list args)
{
  struct tm result;
  time_t const clock = time(0);
  localtime_r(&clock, &result);

  char buff[64];
  size_t n = strftime(buff, sizeof(buff), "%Y-%m-%dT%T", &result);
  if (n > 0)
    buff[n] = '\0';

  // The logger is passed around as a shared_ptr<> to the adapter. There's no telling
  // if multiple threads will log to this thing or not. synchronize to be safe
  std::lock_guard<std::mutex> lock(impl.Mutex);
  fprintf(impl.OutFile, "%s (%5s) Thread-" ThreadId_FMT " [%s:%s:%d] - ", buff, LevelToString(level),
    GetCurrentThreadId(), m_name.c_str(), file, line);
  vfprintf(impl.OutFile, format, args);
  fprintf(impl.OutFile, "\n");
}

std::shared_ptr<adapter::Log>
adapter::Log::GetLog(std::string const& name)
{
  std::shared_ptr<Log> logger;

  std::lock_guard<std::mutex> lock(impl.Mutex);
  LogMap::const_iterator itr = impl.Logs.find(name);
  if (itr == impl.Logs.end())
  {
    logger.reset(new Log(name));
    logger->m_level = impl.DefaultLevel;
    impl.Logs.insert(LogMap::value_type(name, logger));
  }
  else
  {
    logger = itr->second;
  }

  assert(logger.get() != nullptr);
  return logger;
}
