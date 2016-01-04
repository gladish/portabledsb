#pragma once

#if defined(Q_CC_GNU)
#define PRINTF_FORMAT(IDX, FIRST) __attribute__ ((format (printf, IDX, FIRST)))
#else
#define PRINTF_FORMAT(IDX, FIRST)
#endif

#include <stdarg.h>

namespace common
{
  class Logger
  {
  public:
    enum Level
    {
      DSB_LOGLEVEL_OFF = -1,
      DSB_LOGLEVEL_DEBUG = 1,
      DSB_LOGLEVEL_INFO = 2,
      DSB_LOGLEVEL_WARN = 3,
      DSB_LOGLEVEL_ERROR = 4,
      DSB_LOGLEVEL_FATAL = 5
    };

    static void Write(char const* module, Level level, const char* file,
        int line, const char* format, ...) PRINTF_FORMAT(6, 7);

    static bool IsLevelEnabled(const char* module, Level level);

  private:
    static void VaLog(const char* module, Level level, const char* file, int line, const char* format, va_list args);
  };
}

#define DSB_DECLARE_LOGNAME(LOGNAME) char const __dsb_logger_module_name__[] = { #LOGNAME };


#define DSBLOG_WITH_LEVEL(LEVEL, FORMAT, ...) \
    do { if (common::Logger::IsLevelEnabled(__dsb_logger_module_name__, LEVEL)) { \
        common::Logger::Write(__dsb_logger_module_name__, LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)

#define DSBLOG(LEVEL, NAME, FORMAT, ...) \
    do { if (common::Logger::IsLevelEnabled(NAME, common::Logger::LEVEL)) { \
        common::Logger::Write(NAME, common::Logger::LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)


#define DSBLOG_DEBUG(FORMAT, ...) DSBLOG(DSB_LOGLEVEL_DEBUG, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_INFO(FORMAT, ...) DSBLOG(DSB_LOGLEVEL_INFO, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_WARN(FORMAT, ...) DSBLOG(DSB_LOGLEVEL_WARN, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_ERROR(FORMAT, ...) DSBLOG(DSB_LOGLEVEL_ERROR, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)

#define DSBLOG_NOT_IMPLEMENTED() DSBLOG_WARN("%s has not been implemented", __func__);

