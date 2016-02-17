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
#include "AdapterInterface.h"

using namespace adapter;

namespace
{
  template<typename T>
  T const* FindByName(std::vector<T> const& v, char const* name)
  {
    T const* item = nullptr;

    auto itr = std::find_if(v.begin(), v.end(), 
      [&name](T const& obj) { return strcmp(obj.GetName().c_str(), name) == 0; });

    if (itr != v.end())
      item = &(*itr);

    return item;
  }
}

template<typename T>
void Interface::TakeOwnership(std::vector<T>& v, T&& item)
{
  item.SetInterfaceName(GetName());
  v.push_back(std::move(item));
}

Interface::Interface(std::string const& name)
  : Object(name)
{
}

Property const*
Interface::FindProperty(char const* name) const
{
  return FindByName(m_props, name);
}

void
Interface::AddProperty(Property prop)
{
  TakeOwnership(m_props, std::move(prop));
}

Method const*
Interface::FindMethod(char const* name) const
{
  return FindByName(m_methods, name);
}

void
Interface::AddMethod(Method method)
{
  TakeOwnership(m_methods, std::move(method));
}

void
Interface::AddSignal(Signal signal)
{
  TakeOwnership(m_signals, std::move(signal));
}

std::list< std::shared_ptr<Interface> >
Interface::FromFile(std::string const& file)
{
  // TODO:
  return std::list< std::shared_ptr<Interface> >();
}
