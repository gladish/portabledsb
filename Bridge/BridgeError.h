#pragma once

#include "Common/Adapter.h"

#include <sstream>
#include <stdexcept>

namespace bridge
{
  class Error : public std::runtime_error
  {
  public:
    Error(std::string const& message)
      : std::runtime_error(message) { }

    static void Throw(char const* fmt, ...)
    {
      va_list args;
      va_start(args, fmt);
      
      char buff[256];
      vsnprintf(buff, sizeof(buff), fmt, args);
      va_end(args);

      throw Error(buff);
    }

    static void ThrowIfNotOk(QStatus st, char const* fmt, ...)
    {
      if (st == ER_OK)
        return;

      va_list args;
      va_start(args, fmt);

      char buff[256];
      vsnprintf(buff, sizeof(buff), fmt, args);
      va_end(args);

      std::stringstream msg;
      msg << buff;
      msg << ": [QCC] ";
      msg << QCC_StatusText(st);
      throw Error(msg.str());
    }

    static void Throw(adapter::Adapter& a, adapter::Status st, char const* fmt, ...)
    {
      va_list args;
      va_start(args, fmt);

      char buff[256];
      vsnprintf(buff, sizeof(buff), fmt, args);
      va_end(args);

      std::stringstream msg;
      msg << buff;
      msg << ": ";
      msg << a.GetStatusText(st);
      throw Error(msg.str());
    }
  };
}
