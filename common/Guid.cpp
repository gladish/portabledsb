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
#include <common/Guid.h>
#include <string.h>

adapter::Guid::Guid()
{
  memset(m_uuid, 0, sizeof(uuid_t));
}

adapter::Guid::Guid(uuid_t id)
{
  memcpy(m_uuid, id, sizeof(uuid_t));
}

adapter::Guid
adapter::Guid::NewGuid()
{
  uuid_t uuid;
  uuid_generate_random(uuid);
  return Guid(uuid);
}

adapter::Guid
adapter::Guid::Parse(char const* s)
{
  if (s == NULL)
    return Guid::Null();

  uuid_t id;
  return (uuid_parse(s, id) == 0)
    ? Guid(id)
    : Guid::Null();
}

adapter::Guid const&
adapter::Guid::Null()
{
  static Guid zeroGuid = Guid();
  return zeroGuid;
}

uint8_t const* 
adapter::Guid::ToByteArray() const
{
  return &m_uuid[0];
}

std::string
adapter::Guid::ToString() const
{
  char buff[64];
  memset(buff, 0, sizeof(buff));
  uuid_unparse_lower(m_uuid, buff);
  return std::string(buff);
}

bool
adapter::Guid::operator < (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) < 0;
}

bool
adapter::Guid::operator == (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) == 0;
}

bool
adapter::Guid::operator != (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) != 0;
}
