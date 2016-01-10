#include <Common/Guid.h>
#include <string.h>

common::Guid::Guid()
{
  memset(m_uuid, 0, sizeof(uuid_t));
}

common::Guid::Guid(uuid_t id)
{
  memcpy(m_uuid, id, sizeof(uuid_t));
}

common::Guid
common::Guid::NewGuid()
{
#ifdef __linux__
  char id[17];
  memset(id, 0, sizeof(id));
#else
  uuid_string_t id;
#endif

  uuid_t uuid;
  uuid_generate_random(uuid);
  return Guid(uuid);
}

common::Guid
common::Guid::Parse(char const* s)
{
  if (s == NULL)
    return Guid::Null();

  uuid_t id;
  if (uuid_parse(s, id) == 0)
    return Guid(id);
  else
    return Guid::Null();
}

common::Guid const&
common::Guid::Null()
{
  static Guid zeroGuid = Guid();
  return zeroGuid;
}

std::string
common::Guid::ToString() const
{
  char buff[64];
  memset(buff, 0, sizeof(buff));
  uuid_unparse_lower(m_uuid, buff);
  return std::string(buff);
}

bool
common::Guid::operator < (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) < 0;
}

bool
common::Guid::operator == (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) == 0;
}

bool
common::Guid::operator != (Guid const& rhs) const
{
  return uuid_compare(m_uuid, rhs.m_uuid) != 0;
}
