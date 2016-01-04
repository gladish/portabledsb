
#include "Common/Log.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include <map>

namespace
{
  FILE* gLog = stdout;

  struct StringCompare
  {
    bool operator()(char const* s, char const* t) const
      { return strcmp(s, t) < 0; }
  };

  char const* LevelToString(common::Logger::Level l)
  {
    char const* s = "unknown";
    switch (l)
    {
      case common::Logger::DSB_LOGLEVEL_OFF:
        s = "OFF";
        break;
      case common::Logger::DSB_LOGLEVEL_DEBUG:
        s = "DEBUG";
        break;
      case common::Logger::DSB_LOGLEVEL_INFO:
        s = "INFO";
        break;
      case common::Logger::DSB_LOGLEVEL_WARN:
        s = "WARN";
        break;
      case common::Logger::DSB_LOGLEVEL_ERROR:
        s = "ERROR";
        break;
      default:
        break;
    }
    return s;
  }

  typedef std::map<char const*, common::Logger::Level, StringCompare> LevelMap;
  LevelMap gLevelMap;

  common::Logger::Level sDefaultLoggingLevel = common::Logger::DSB_LOGLEVEL_INFO;

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
common::Logger::Write(char const* module, Level level, char const* file,
        int line, char const* format, ...)
{ 
  va_list args;
  va_start(args, format);
  VaLog(module, level, file, line, format, args);
  va_end(args);
}

bool
common::Logger::IsLevelEnabled(char const* module, Level level)
{
  common::Logger::Level minLevel = sDefaultLoggingLevel;
  LevelMap::const_iterator itr = gLevelMap.find(module);
  if (itr != gLevelMap.end())
    minLevel = itr->second;
  
  return level >= minLevel;
}

void
common::Logger::VaLog(const char* module, Level level, const char* file, int line,
  const char* format, va_list args)
{
  struct tm result;
  time_t const clock = time(0);
  localtime_r(&clock, &result);

  char buff[64];
  size_t n = strftime(buff, sizeof(buff) - 1, "%Y-%m-%dT%T", &result);
  if (n > 0)
    buff[n] = '\0';
  fprintf(gLog, "%s (%5s) Thread-" ThreadId_FMT " [%s:%s:%d] - ", buff, LevelToString(level), GetCurrentThreadId(), module, file, line);
  vfprintf(gLog, format, args);
  fprintf(gLog, "\n");
}

