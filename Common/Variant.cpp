#include "Variant.h"
#include <string.h>
#include <sstream>

namespace
{
  template<class TFrom, class TOut>
  void printArray(std::stringstream& buff, void* arr, int n)
  {
    TFrom* typed_array = reinterpret_cast<TFrom *>(arr);
    for (int i = 0; i < n; ++i)
    {
      if (i > 0) buff << ',';
      buff << static_cast<TOut>(typed_array[i]);
    }
  }

  template<class T>
  void releaseArray(void* p)
  {
    T* arr = reinterpret_cast<T *>(p);
    delete [] arr;
  }

  void freeArray2(void* p, adapter::TypeId t)
  {
    typedef adapter::TypeId dt;
    switch (t)
    {
      case dt::BooleanArray:  releaseArray<bool>(p); break;
      case dt::UInt8Array:    releaseArray<uint8_t>(p); break;
      case dt::Int16Array:    releaseArray<int16_t>(p); break;
      case dt::UInt16Array:   releaseArray<uint16_t>(p); break;
      case dt::Int32Array:    releaseArray<int32_t>(p); break;
      case dt::UInt32Array:   releaseArray<uint32_t>(p); break;
      case dt::Int64Array:    releaseArray<int64_t>(p); break;
      case dt::UInt64Array:   releaseArray<uint64_t>(p); break;
      case dt::DoubleArray:   releaseArray<double>(p); break;
      case dt::StringArray:   releaseArray<std::string>(p); break;
      default:
        assert(false);
        break;
    }
  }

  template<class T>
  int vectorCompare(void* arg1, int n1, void* arg2, int n2)
  {
    T const* v1 = reinterpret_cast< T const * >(arg1);
    T const* v2 = reinterpret_cast< T const * >(arg2);

    if (!v1&& v2) return -1;
    if (v1&& !v2) return 1;

    if (n1 < n2) return -1;
    if (n1 > n2) return 1;

    for (int i = 0; i < n1; ++i)
    {
      if (v1[i] != v2[i])
        return v1[i] < v2[i] ? -1 : 1;
    }

    return 0;
  }
}

adapter::Variant::Data::Data()
{
  Type = TypeId::Null;
  memset(&Item, 0, sizeof(m_data.Item));
  Size = 0;
}

adapter::Variant::Variant()
  : m_data()
{
}

adapter::Variant::Variant(bool b)
  : m_data()
{
  m_data.Type = TypeId::Boolean;
  m_data.Item.v_bool = b;
}

adapter::Variant::Variant(std::vector<bool> const& v)
  : m_data()
{
  m_data.Type = TypeId::BooleanArray;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(uint8_t y)
  : m_data()
{
  m_data.Type = TypeId::UInt8;
  m_data.Item.v_uint8 = y;
}

adapter::Variant::Variant(std::vector<uint8_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::UInt8Array;
  AllocAndCopy(m_data, v);
}


adapter::Variant::Variant(int16_t n)
  : m_data()
{
  m_data.Type = TypeId::Int16;
  m_data.Item.v_int16 = n;
}

adapter::Variant::Variant(std::vector<uint16_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::UInt16Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(uint16_t q)
  : m_data()
{
  m_data.Type = TypeId::UInt16;
  m_data.Item.v_uint16 = q;
}

adapter::Variant::Variant(std::vector<int16_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::Int16Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(int32_t i)
  : m_data()
{
  m_data.Type = TypeId::Int32;
  m_data.Item.v_int32 = i;
}

adapter::Variant::Variant(std::vector<int32_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::Int32Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(uint32_t u)
  : m_data()
{
  m_data.Type = TypeId::UInt32;
  m_data.Item.v_uint32 = u;
}

adapter::Variant::Variant(std::vector<uint32_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::UInt32Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(int64_t x)
  : m_data()
{
  m_data.Type = TypeId::Int64;
  m_data.Item.v_int64 = x;
}

adapter::Variant::Variant(std::vector<int64_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::Int64Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(uint64_t t)
  : m_data()
{
  m_data.Type = TypeId::UInt64;
  m_data.Item.v_uint64 = t;
}

adapter::Variant::Variant(std::vector<uint64_t> const& v)
  : m_data()
{
  m_data.Type = TypeId::UInt64Array;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(double d)
  : m_data()
{
  m_data.Type = TypeId::Double;
  m_data.Item.v_double = d;
}

adapter::Variant::Variant(std::vector<double> const& v)
  : m_data()
{
  m_data.Type = TypeId::DoubleArray;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(TypeId t)
  : m_data()
{
  m_data.Type = t;
}

adapter::Variant::Variant(std::string const& s)
  : m_data()
{
  m_data.Type = TypeId::String;
  if (!m_data.Item.v_string)
    m_data.Item.v_string = new std::string();
  *m_data.Item.v_string = s;
}

adapter::Variant::Variant(char const* s)
  : m_data()
{
  m_data.Type = TypeId::String;
  if (s)
  {
    if (!m_data.Item.v_string)
      m_data.Item.v_string = new std::string();
    *m_data.Item.v_string = s;
  }
}

adapter::Variant::Variant(std::vector<std::string> const& v)
  : m_data()
{
  m_data.Type = TypeId::StringArray;
  AllocAndCopy(m_data, v);
}

adapter::Variant::Variant(Variant const& other)
{
  AssignFrom(other);
}

adapter::Variant::~Variant()
{
  if (m_data.Type == TypeId::String && m_data.Item.v_string)
    delete m_data.Item.v_string;

  if (IsArray() && m_data.Item.v_arr)
    freeArray2(m_data.Item.v_arr, m_data.Type);
}

adapter::Variant&
adapter::Variant::operator=(Variant const& other)
{
  if (&other != this)
    AssignFrom(other);
  return *this;
}

bool
adapter::Variant::IsArray() const
{
  // relies on knowing order of enum
  return GetType() > TypeId::String;
}

int
adapter::Variant::Length() const
{
  if (!IsArray()) return -1;
  return m_data.Size;
}

std::vector<bool>
adapter::Variant::ToBooleanArray(bool* ok) const
{
  return GetArray<bool>(TypeId::BooleanArray, ok);
}

std::vector<uint8_t>
adapter::Variant::ToUInt8Array(bool* ok) const
{
  return GetArray<uint8_t>(TypeId::UInt8Array, ok);
}

std::vector<int16_t>
adapter::Variant::ToInt16Array(bool* ok) const
{
  return GetArray<int16_t>(TypeId::Int16Array, ok);
}

std::vector<uint16_t>
adapter::Variant::ToUInt16Array(bool* ok) const
{
  return GetArray<uint16_t>(TypeId::UInt16Array, ok);
}

std::vector<int32_t>
adapter::Variant::ToInt32Array(bool* ok) const
{
  return GetArray<int32_t>(TypeId::Int32Array, ok);
}

std::vector<uint32_t>
adapter::Variant::ToUInt32Array(bool* ok) const
{
  return GetArray<uint32_t>(TypeId::UInt32Array, ok);
}

std::vector<int64_t>
adapter::Variant::ToInt64Array(bool* ok) const
{
  return GetArray<int64_t>(TypeId::Int64Array, ok);
}

std::vector<uint64_t>
adapter::Variant::ToUInt64Array(bool* ok) const
{
  return GetArray<uint64_t>(TypeId::UInt64Array, ok);
}

std::vector<double>
adapter::Variant::ToDoubleArray(bool* ok) const
{
  return GetArray<double>(TypeId::DoubleArray, ok);
}

std::vector<std::string>
adapter::Variant::ToStringArray(bool* ok) const
{
  return GetArray<std::string>(TypeId::StringArray, ok);
}

void
adapter::Variant::AssignFrom(Variant const& other)
{
  switch (other.m_data.Type)
  {
    case TypeId::Null: break;
    case TypeId::Boolean: m_data.Item.v_bool = other.m_data.Item.v_bool; break;
    case TypeId::UInt8:   m_data.Item.v_uint8 = other.m_data.Item.v_uint8; break;
    case TypeId::Int16:   m_data.Item.v_int16 = other.m_data.Item.v_int16; break;
    case TypeId::UInt16:  m_data.Item.v_uint16 = other.m_data.Item.v_uint16; break;
    case TypeId::Int32:   m_data.Item.v_int32 = other.m_data.Item.v_int32; break;
    case TypeId::UInt32:  m_data.Item.v_uint32 = other.m_data.Item.v_uint32; break;
    case TypeId::Int64:   m_data.Item.v_int64 = other.m_data.Item.v_int64; break;
    case TypeId::UInt64:  m_data.Item.v_uint64 = other.m_data.Item.v_uint64; break;
    case TypeId::Double:  m_data.Item.v_double = other.m_data.Item.v_double; break;
    case TypeId::String:
    {
      if (other.m_data.Item.v_string)
      {
        if (!m_data.Item.v_string)
          m_data.Item.v_string = new std::string();
        *m_data.Item.v_string = *other.m_data.Item.v_string;
      }
    }
    break;

    // arrays
    case TypeId::BooleanArray:
      AssignFromArray<bool>(m_data, other.m_data);
      break;

    case TypeId::UInt8Array:
      AssignFromArray<uint8_t>(m_data, other.m_data);
      break;

    case TypeId::Int16Array:
      AssignFromArray<int16_t>(m_data, other.m_data);
      break;

    case TypeId::UInt16Array:
      AssignFromArray<uint16_t>(m_data, other.m_data);
      break;

    case TypeId::Int32Array:
      AssignFromArray<int32_t>(m_data, other.m_data);
      break;

    case TypeId::UInt32Array:
      AssignFromArray<uint32_t>(m_data, other.m_data);
      break;

    case TypeId::Int64Array:
      AssignFromArray<int64_t>(m_data, other.m_data);
      break;

    case TypeId::UInt64Array:
      AssignFromArray<uint64_t>(m_data, other.m_data);
      break;

    case TypeId::DoubleArray:
      AssignFromArray<double>(m_data, other.m_data);
      break;

    case TypeId::StringArray:
      AssignFromArray<std::string>(m_data, other.m_data);
      break;
  }
  m_data.Type = other.m_data.Type;
}

bool
adapter::Variant::CanConvert(TypeId t) const
{
  if (m_data.Type == t)
    return true;

  bool canConvert = false;
  switch (m_data.Type)
  {
    case TypeId::Null:  break;
    case TypeId::Boolean: if (!canConvert) canConvert = (t == TypeId::Boolean);
    case TypeId::UInt8:   if (!canConvert) canConvert = (t == TypeId::UInt8);
    case TypeId::Int16:   if (!canConvert) canConvert = (t == TypeId::Int16);
    case TypeId::UInt16:  if (!canConvert) canConvert = (t == TypeId::UInt16);
    case TypeId::Int32:   if (!canConvert) canConvert = (t == TypeId::Int32);
    case TypeId::UInt32:  if (!canConvert) canConvert = (t == TypeId::UInt32);
    case TypeId::Int64:   if (!canConvert) canConvert = (t == TypeId::Int64);
    case TypeId::UInt64:  if (!canConvert) canConvert = (t == TypeId::UInt64);
      break;
    default: assert(false);
      break;
  }
  return canConvert;
}

std::string
adapter::Variant::ToString() const
{
  if (IsArray())
  {
    std::stringstream buff;
    buff << '[';
    switch (m_data.Type)
    {
      case TypeId::Null: break;
      case TypeId::BooleanArray:  printArray<bool, bool>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::UInt8Array:    printArray<uint8_t, int>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::Int16Array:    printArray<int16_t, int>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::UInt16Array:   printArray<uint16_t, int>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::Int32Array:    printArray<int32_t, int>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::UInt32Array:   printArray<uint32_t, uint32_t>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::Int64Array:    printArray<int64_t, int64_t>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::UInt64Array:   printArray<uint64_t, uint64_t>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::DoubleArray:   printArray<double, double>(buff, m_data.Item.v_arr, m_data.Size); break;
      case TypeId::StringArray:   printArray<std::string, std::string>(buff, m_data.Item.v_arr, m_data.Size); break;
      default:
        assert(false);
        break;
    }
    buff << ']';
    return buff.str();
  }

  if (m_data.Type == TypeId::String)
  {
    return (m_data.Item.v_string != NULL)
      ? *m_data.Item.v_string
      : std::string();
  }

  std::stringstream buff;
  switch (m_data.Type)
  {
    case TypeId::Boolean:   buff << (m_data.Item.v_bool ? "true" : "false"); break;
    case TypeId::UInt8:     buff << (int) m_data.Item.v_uint8; break;
    case TypeId::Int16:     buff << m_data.Item.v_int16; break;
    case TypeId::UInt16:    buff << m_data.Item.v_uint16; break;
    case TypeId::Int32:     buff << m_data.Item.v_int32; break;
    case TypeId::UInt32:    buff << m_data.Item.v_uint32; break;
    case TypeId::Int64:     buff << m_data.Item.v_int64; break;
    case TypeId::UInt64:    buff << m_data.Item.v_uint64; break;
    case TypeId::Double:    buff << m_data.Item.v_double; break;
    default:
      assert(false);
      break;
  }
  return buff.str();
}

bool
adapter::Variant::operator < (Variant const& rhs) const
{
  bool lessThan = false;
  if (m_data.Type == rhs.m_data.Type)
  {
    switch (m_data.Type)
    {
      case TypeId::Null:
        lessThan = false;
        break;
      case TypeId::Boolean:
      case TypeId::UInt8:
      case TypeId::Int16:
      case TypeId::UInt16:
      case TypeId::Int32:
      case TypeId::UInt32:
      case TypeId::Int64:
      case TypeId::UInt64:
        lessThan = m_data.Item.v_uint64 < rhs.m_data.Item.v_uint64;
        break;
      case TypeId::Double:
        lessThan = m_data.Item.v_double < rhs.m_data.Item.v_double;
        break;
      case TypeId::String:
        if (m_data.Item.v_string == NULL)
          lessThan = rhs.m_data.Item.v_string != NULL;
        else if (rhs.m_data.Item.v_string == NULL)
          lessThan = false;
        else
          lessThan = *m_data.Item.v_string < *rhs.m_data.Item.v_string;
        break;
      case TypeId::BooleanArray:
        lessThan = vectorCompare<bool>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::UInt8Array:
        lessThan = vectorCompare<uint8_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::Int16Array:
        lessThan = vectorCompare<int16_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;;
        break;
      case TypeId::UInt16Array:
        lessThan = vectorCompare<uint16_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::Int32Array:
        lessThan = vectorCompare<int32_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::UInt32Array:
        lessThan = vectorCompare<uint32_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::Int64Array:
        lessThan = vectorCompare<int64_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::UInt64Array:
        lessThan = vectorCompare<uint64_t>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::DoubleArray:
        lessThan = vectorCompare<double>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      case TypeId::StringArray:
        lessThan = vectorCompare<std::string>(m_data.Item.v_arr, m_data.Size, rhs.m_data.Item.v_arr, rhs.m_data.Size) < 0;
        break;
      default:
        assert(false);
        break;
    }
  }
  else
  {
    if (m_data.Type == TypeId::Null)
      lessThan = rhs.m_data.Type != TypeId::Null;
    else
      lessThan = m_data.Type < rhs.m_data.Type;
  }
  return lessThan;
}
