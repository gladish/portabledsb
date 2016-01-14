
#include "Common/Log.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <mutex>

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include <map>

namespace
{
  FILE* gLog = stdout;

  std::mutex gLogMutex;

  char const* LevelToString(common::LogLevel l)
  {
    char const* s = "unknown";
    switch (l)
    {
      case common::LogLevel::Off: s = "OFF"; break;
      case common::LogLevel::Debug: s = "DEBUG"; break;
      case common::LogLevel::Info: s = "INFO"; break;
      case common::LogLevel::Warn: s = "WARN"; break;
      case common::LogLevel::Error: s = "ERROR"; break;
      case common::LogLevel::Fatal: s = "FATAL"; break;
      default:
        break;
    }
    return s;
  }

  typedef std::map<std::string, common::LogLevel> LevelMap;
  LevelMap gLevelMap;

  common::LogLevel const kDefaultLoggingLevel = common::LogLevel::Info;

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

void
common::Logger::SetLevel(std::string const& module, LogLevel level)
{
  // TODO: does this need to be thread-safe?
  gLevelMap[module] = level;
}

void
common::Logger::Write(std::string const& module, LogLevel level, char const* file,
        int line, char const* format, ...)
{ 
  va_list args;
  va_start(args, format);
  VaLog(module, level, file, line, format, args);
  va_end(args);
}

bool
common::Logger::IsLevelEnabled(std::string const& module, LogLevel level)
{
  common::LogLevel minLevel = kDefaultLoggingLevel;
  LevelMap::const_iterator itr = gLevelMap.find(module);
  if (itr != gLevelMap.end())
    minLevel = itr->second;
  return level >= minLevel;
}

void
common::Logger::VaLog(std::string const& module, LogLevel level, const char* file, int line,
  const char* format, va_list args)
{
  struct tm result;
  time_t const clock = time(0);
  localtime_r(&clock, &result);

  // TODO: use std:: if possible
  char buff[64];
  size_t n = strftime(buff, sizeof(buff) - 1, "%Y-%m-%dT%T", &result);
  if (n > 0)
    buff[n] = '\0';

  // The logger is passed around as a shared_ptr<> to the adapter. There's no telling
  // if multiple threads will log to this thing or not. synchronize to be safe
  std::lock_guard<std::mutex> lock(gLogMutex);
  fprintf(gLog, "%s (%5s) Thread-" ThreadId_FMT " [%s:%s:%d] - ", buff, LevelToString(level),
    GetCurrentThreadId(), module.c_str(), file, line);
  vfprintf(gLog, format, args);
  fprintf(gLog, "\n");
}
