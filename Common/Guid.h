#pragma once

#include <uuid/uuid.h>
#include <string>

namespace adapter
{
  class Guid
  {
  public:
    static Guid const& Null();

    static Guid NewGuid();
    static Guid Parse(char const* s);

    std::string ToString() const;

    uint8_t const* ToByteArray() const;

    bool operator <  (Guid const& rhs) const;
    bool operator == (Guid const& rhs) const;
    bool operator != (Guid const& rhs) const;

  private:
    Guid();
    Guid(uuid_t id);

  private:
    uuid_t m_uuid;
  };
}
