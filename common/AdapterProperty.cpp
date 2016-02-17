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
#include "AdapterProperty.h"

using namespace adapter;

Property::Property(std::string const& name, TypeId type, AccessType access) 
  : adapter::Object(name)
  , m_type(type)
  , m_access(access)
{
}

Property
adapter::MakeProperty(std::string const& name, TypeId type, AccessType access, NamedValue::Vector const& attrs)
{
  Property prop(name, type, access);
  for (auto const& attr : attrs)
    prop.AddAttribute(attr.GetName(), attr.GetValue());
  return std::move(prop);
}
