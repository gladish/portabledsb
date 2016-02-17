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

#include <atomic>
#include <map>
#include <stdint.h>

#include "Value.h"

namespace adapter
{
  typedef std::map< std::string, Value > AttributeMap;

  class Object
  {
  public:
    Object(std::string const& name)
      : m_name(name)
      , m_id(GetNextObjectId())
    {
    }

    virtual ~Object() { }

    uint64_t GetId() const
      { return m_id; }

    std::string const& GetName() const
      { return m_name; }

    std::string const& GetDescription() const
      { return m_description; }

    void SetDescription(std::string const& desc)
      { m_description = desc; }

    AttributeMap const& GetAttributes() const
      { return m_attrs; }

    Value GetAttribute(std::string const& name) const
    {
      auto itr = m_attrs.find(name);
      return itr != m_attrs.end() ? itr->second : Value();
    }

    void AddAttribute(std::string const& name, Value const& value)
      { m_attrs.insert(AttributeMap::value_type(name, value)); }

    void ClearAttributes()
      { m_attrs.clear(); }

  private:
    std::string           m_name;
    uint64_t              m_id;
    AttributeMap   m_attrs;
    std::string           m_description;

  private:
    uint64_t GetNextObjectId()
    {
      static std::atomic<uint64_t> s_id;
      return s_id++;
    }
  };
}
