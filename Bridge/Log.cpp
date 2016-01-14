#include "Bridge/Log.h"
#include "Common/Log.h"

namespace
{
  common::LogLevel toLogLevel(bridge::AdapterLogLevel level)
  {
    common::LogLevel l = common::LogLevel::Off;
    switch (level)
    {
      case bridge::AdapterLogLevel::Off:    l = common::LogLevel::Off; break;
      case bridge::AdapterLogLevel::Debug:  l = common::LogLevel::Debug; break;
      case bridge::AdapterLogLevel::Info:   l = common::LogLevel::Info; break;
      case bridge::AdapterLogLevel::Warn:   l = common::LogLevel::Warn; break;
      case bridge::AdapterLogLevel::Error:  l = common::LogLevel::Error; break;
      case bridge::AdapterLogLevel::Fatal:  l = common::LogLevel::Fatal; break;
    }
    return l;
  }
}

bridge::AdapterLog::AdapterLog(std::string const& module)
  : m_module(module)
{
}

void
bridge::AdapterLog::Write(AdapterLogLevel level, char const* file, int line, char const* format, ...)
{
  va_list args;
  va_start(args, format);
  common::Logger::VaLog(m_module.c_str(), toLogLevel(level), file, line, format, args);
  va_end(args);
}

bool
bridge::AdapterLog::IsLevelEnabled(AdapterLogLevel level)
{
  common::LogLevel l = toLogLevel(level);
  return common::Logger::IsLevelEnabled(m_module.c_str(), l);
}

