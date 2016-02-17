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

#include "AdapterMethod.h"
#include "AdapterObject.h"
#include "AdapterProperty.h"
#include "AdapterSignal.h"
#include <algorithm>
#include <list>
#include <vector>

namespace adapter
{
  class Interface : public adapter::Object
  {
  public:
    typedef std::vector<Interface> Vector;

    Interface(std::string const& name);

    // properties
    void AddProperty(Property prop);

    inline void ClearProperties()
      { m_props.clear(); }

    inline Property::Vector const& GetProperties() const
      { return m_props; }

    Property const* FindProperty(char const* name) const;


    // methods
    void AddMethod(Method method);

    inline void ClearMethods()
      { m_methods.clear(); }

    inline Method::Vector const& GetMethods() const
      { return m_methods; }

    Method const* FindMethod(char const* name) const;


    // signals
    void AddSignal(Signal signal);

    inline void ClearSignals()
      { m_signals.clear(); }

    inline Signal::Vector const& GetSignals() const
      { return m_signals; }


    static std::list< std::shared_ptr<Interface> > FromFile(std::string const& file);

  private:
    template<typename T>
    void TakeOwnership(std::vector<T>& v, T&& item);

  private:
    Property::Vector m_props;
    Method::Vector   m_methods;
    Signal::Vector   m_signals;
  };
}
