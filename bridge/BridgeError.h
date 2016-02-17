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

#include "common/Adapter.h"

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
