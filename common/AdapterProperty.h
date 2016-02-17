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
#include <string>

#include "AdapterObject.h"
#include "AdapterNamedValue.h"
#include "AccessType.h"
#include "SignalBehavior.h"
#include "TypeId.h"

namespace adapter
{
  class Interface;

  class Property : public adapter::Object
  {
    friend class Interface;

  public:
    typedef std::vector<Property> Vector;

    Property(std::string const& name, TypeId type, AccessType access);

    inline std::string const& GetInterfaceName() const
      { return m_interface; }

    inline TypeId GetType() const
      { return m_type; }

    inline AccessType GetAccess() const
      { return m_access; }

    inline void SetAccess(AccessType access)
      { m_access = access; }

    inline SignalBehavior GetChangeOfValueBehavior() const
      { return m_changeOfValueBehavior; }

    inline void SetGetChangeOfValueBehavior(SignalBehavior changeOfValueBehavior)
      { m_changeOfValueBehavior = changeOfValueBehavior; }

  private:
    void SetInterfaceName(std::string const& name)
      { m_interface = name; }

  private:
    std::string           m_interface;
    TypeId                m_type;
    AccessType            m_access;
    SignalBehavior        m_changeOfValueBehavior;
  };

  Property MakeProperty(std::string const& name, TypeId type,
    AccessType access, NamedValue::Vector const& attrs = NamedValue::Vector());
}
