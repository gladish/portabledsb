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

#include "Value.h"

#include <memory>
#include <vector>

namespace adapter
{
  class NamedValue
  {
  public:
    typedef std::vector<NamedValue> Vector;

    NamedValue()
    {
    }

    NamedValue(std::string const& name, Value const& value);

    inline std::string const& GetName() const
      { return m_name; }

    inline Value const& GetValue() const
      { return m_value; }

    inline void SetName(std::string const& name)
      { m_name = name; }

    inline void SetValue(Value const& value)
      { m_value = value; }

    inline bool IsNull() const
      { return m_name.empty() && m_value.IsEmpty(); }

    static NamedValue const& Null()
    {
      static NamedValue kNullValue("", Value());
      return kNullValue;
    }

  private:
    std::string   m_name;
    Value       m_value;
  };
}
