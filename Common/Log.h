#pragma once

#if defined(Q_CC_GNU)
#define PRINTF_FORMAT(IDX, FIRST) __attribute__ ((format (printf, IDX, FIRST)))
#else
#define PRINTF_FORMAT(IDX, FIRST)
#endif

#include <stdarg.h>
#include <string>

namespace common
{
  enum class LogLevel
  {
    Off,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
  };

  class Logger
  {
  public:
    static void SetLevel(std::string const& module, LogLevel level);

    static void Write(std::string const& module, LogLevel level, const char* file,
        int line, const char* format, ...) PRINTF_FORMAT(6, 7);

    static bool IsLevelEnabled(std::string const& module, LogLevel level);

    static void VaLog(std::string const& module, LogLevel level, const char* file,
      int line, const char* format, va_list args);
  };
}

#define DSB_DECLARE_LOGNAME(LOGNAME) std::string const __dsb_logger_module_name__ = #LOGNAME

#define DSBLOG_WITH_LEVEL(LEVEL, FORMAT, ...) \
    do { if (common::Logger::IsLevelEnabled(__dsb_logger_module_name__, LEVEL)) { \
        common::Logger::Write(__dsb_logger_module_name__, LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)

#define DSBLOG(LEVEL, NAME, FORMAT, ...) \
    do { if (common::Logger::IsLevelEnabled(NAME, common::LogLevel::LEVEL)) { \
        common::Logger::Write(NAME, common::LogLevel::LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)


#define DSBLOG_DEBUG(FORMAT, ...) DSBLOG(Debug, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_INFO(FORMAT, ...) DSBLOG(Info, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_WARN(FORMAT, ...) DSBLOG(Warn, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_ERROR(FORMAT, ...) DSBLOG(Error, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)

#define DSBLOG_NOT_IMPLEMENTED() DSBLOG_WARN("%s has not been implemented", __func__);

