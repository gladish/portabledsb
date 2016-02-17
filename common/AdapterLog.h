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
#pragma once

#if defined(Q_CC_GNU)
#define PRINTF_FORMAT(IDX, FIRST) __attribute__ ((format (printf, IDX, FIRST)))
#else
#define PRINTF_FORMAT(IDX, FIRST)
#endif

#include <stdarg.h>
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>

namespace adapter 
{
  enum class LogLevel
  {
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Off
  };

  class Log
  {
  public:
    static std::shared_ptr<Log> GetLog(std::string const& name);

    Log(std::string const& name);

    void Write(LogLevel level, const char* file, int line, const char* format, ...)
      PRINTF_FORMAT(5, 6);

    void VaLog(LogLevel level, const char* file, int line, const char* format, va_list args);

    bool IsLevelEnabled(LogLevel level);

    std::string const& GetName() const
      { return m_name; }

    void SetLevel(LogLevel level);

    static void SetDefaultLevel(LogLevel level);
  private:
    LogLevel    m_level;
    std::string        m_name;
  };
}

#define DSB_DECLARE_LOGNAME(LOGNAME) std::string const __dsb_logger_module_name__ = #LOGNAME

#define DSBLOG_WITH_LEVEL(LEVEL, FORMAT, ...) \
    do { std::shared_ptr<adapter::Log> log = adapter::Log::GetLog(__dsb_logger_module_name__); \
      if (log->IsLevelEnabled(LEVEL)) { \
        log->Write(LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)

#define DSBLOG_LEVEL_ENABLED(LEVEL) adapter::Log::GetLog(__dsb_logger_module_name__)->IsLevelEnabled(adapter::LogLevel::LEVEL)

#define DSBLOG(LEVEL, NAME, FORMAT, ...) \
    do { std::shared_ptr<adapter::Log> log = adapter::Log::GetLog(NAME); \
      if (log->IsLevelEnabled(adapter::LogLevel::LEVEL)) { \
        log->Write(adapter::LogLevel::LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
    } } while (0)


#define DSBLOG_DEBUG(FORMAT, ...) DSBLOG(Debug, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_INFO(FORMAT, ...) DSBLOG(Info, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_WARN(FORMAT, ...) DSBLOG(Warn, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)
#define DSBLOG_ERROR(FORMAT, ...) DSBLOG(Error, __dsb_logger_module_name__, FORMAT, ##__VA_ARGS__)

#define DSBLOG_NOT_IMPLEMENTED() DSBLOG_WARN("%s has not been implemented", __func__);

class NotImplementedException : std::runtime_error
{
public:
  NotImplementedException(char const* func, char const* file, int line)
    : std::runtime_error(CreateMessage(func, file, line)) { }
private:
  static std::string CreateMessage(char const* func, char const* file, int line)
  {
    std::stringstream buff;
    buff << "not implemented:";
    if (func)
      buff << func;
    if (file)
    {
      buff << " file:" << file;
      buff << ":" << line;
    }
    return buff.str();
  }
};

#define DSBLOG_ASSERT_NOT_IMPLEMENTED() throw NotImplementedException(__func__, __FILE__, __LINE__)

