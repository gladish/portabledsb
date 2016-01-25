#pragma once

#include "Common/Assert.h"
#include "Common/TypeId.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ostream>
#include <string>
#include <vector>

namespace adapter 
{
  class Variant
  {
  public:

    Variant();
    Variant(bool b);
    Variant(uint8_t y);
    Variant(int16_t n);
    Variant(uint16_t q);
    Variant(int32_t i);
    Variant(uint32_t u);
    Variant(int64_t x);
    Variant(uint64_t t);
    Variant(double d);
    Variant(TypeId typeId);
    Variant(char const* s);
    Variant(std::string const& s);
    Variant(Variant const& other);

    Variant(std::vector<bool> const& v);
    Variant(std::vector<uint8_t> const& v);
    Variant(std::vector<int16_t> const& v);
    Variant(std::vector<uint16_t> const& v);
    Variant(std::vector<int32_t> const& v);
    Variant(std::vector<uint32_t> const& v);
    Variant(std::vector<int64_t> const& v);
    Variant(std::vector<uint64_t> const& v);
    Variant(std::vector<double> const& v);
    Variant(std::vector<std::string> const& v);

    Variant& operator=(Variant const& other);

    ~Variant();

    bool operator < (Variant const& rhs) const;
    // inline bool operator!=(const X& lhs, const X& rhs){return !operator==(lhs,rhs);}
    // inline bool operator==(const X& lhs, const X& rhs){ /* do actual comparison */ }


    bool IsArray() const;
    int  Length() const;

    inline bool IsEmpty() const
      { return m_data.Size == 0 && m_data.Type == TypeId::Null; }

    inline TypeId GetType() const
      { return m_data.Type; }

    inline bool ToBoolean(bool* ok = NULL) const
      { return Get<bool>(TypeId::Boolean, ok); }

    inline uint8_t ToUInt8(bool* ok = NULL) const
      { return Get<uint8_t>(TypeId::UInt8, ok); }

    inline int16_t ToInt16(bool* ok = NULL) const
      { return Get<int16_t>(TypeId::Int16, ok); }

    inline uint16_t ToUInt16(bool* ok = NULL) const
      { return Get<uint16_t>(TypeId::UInt16, ok); }

    inline int32_t ToInt32(bool* ok = NULL) const
      { return Get<int32_t>(TypeId::Int32, ok); }

    inline uint32_t ToUInt32(bool* ok = NULL) const
      { return Get<uint32_t>(TypeId::UInt32, ok); }

    inline int64_t ToInt64(bool* ok = NULL) const
      { return Get<int64_t>(TypeId::Int64, ok); }

    inline uint64_t ToUInt64(bool* ok = NULL) const
      { return Get<uint64_t>(TypeId::UInt64, ok); }

    inline double ToDouble(bool* ok = NULL) const
      { return Get<uint16_t>(TypeId::Double, ok); }

    std::vector<bool>         ToBooleanArray(bool* ok = NULL) const;
    std::vector<uint8_t>      ToUInt8Array(bool* ok = NULL) const;
    std::vector<int16_t>      ToInt16Array(bool* ok = NULL) const;
    std::vector<uint16_t>     ToUInt16Array(bool* ok = NULL) const;
    std::vector<int32_t>      ToInt32Array(bool* ok = NULL) const;
    std::vector<uint32_t>     ToUInt32Array(bool* ok = NULL) const;
    std::vector<int64_t>      ToInt64Array(bool* ok = NULL) const;
    std::vector<uint64_t>     ToUInt64Array(bool* ok = NULL) const;
    std::vector<double>       ToDoubleArray(bool* ok = NULL) const;
    std::vector<std::string>  ToStringArray(bool* ok = NULL) const;

    std::string ToString() const;

    bool CanConvert(TypeId t) const;

    friend std::ostream& operator << (std::ostream& os, Variant const& v);

  private:
    template<class T>
    static void* allocArray(int n)
    {
      T* p = new T[n];
      return reinterpret_cast<void *>(p);
    }

    template<class T>
    static void freeArray(void* p)
    {
      T* arr = reinterpret_cast<T *>(p);
      delete [] arr;
    }

    template<class T>
    T Convert(TypeId t) const
    {
      // no conversion for now
      switch (t)
      {
        case TypeId::Boolean: return m_data.Item.v_bool;
        case TypeId::UInt8:   return m_data.Item.v_uint8;
        case TypeId::Int16:   return m_data.Item.v_int16;
        case TypeId::UInt16:  return m_data.Item.v_uint16;
        case TypeId::Int32:   return m_data.Item.v_int32;
        case TypeId::UInt32:  return m_data.Item.v_uint32;
        case TypeId::Int64:   return m_data.Item.v_int64;
        case TypeId::UInt64:  return m_data.Item.v_uint64;
        case TypeId::Double:  return m_data.Item.v_double;
        default:
          DSB_ASSERT(false);
          break;
      }
    }

    template<class T>
    T Get(TypeId t, bool* ok) const
    {
      if (CanConvert(t))
      {
        if (ok) *ok = true;
        return Convert<T>(t);
      }
      if (ok) *ok = false;
      return T();
    }

    template<class T>
    std::vector<T> GetArray(TypeId t, bool *ok) const
    {
      if (CanConvert(t))
      {
        T* arr = (T *) m_data.Item.v_arr;

        std::vector<T> v;
        for (int i = 0; i < m_data.Size; ++i)
          v.push_back(arr[i]);
        return v;
      }
      if (ok)
        *ok = false;
      return std::vector<T>();
    }

    union DataItem {
      bool          v_bool;
      uint8_t       v_uint8;
      int16_t       v_int16;
      uint16_t      v_uint16;
      int32_t       v_int32;
      uint32_t      v_uint32;
      int64_t       v_int64;
      uint64_t      v_uint64;
      double        v_double;
      std::string*  v_string;

      // all arrays are stashed here
      void*         v_arr;
    };

    struct Data
    {
      TypeId Type;
      DataItem Item;
      int      Size;

      Data();
    };

    Data m_data;

    void AssignFrom(Variant const& other);

    template<class T>
    void AssignFromArray(Data& to, Data const& from)
    {
      if (IsArray() && to.Item.v_arr)
        freeArray<T>(to.Item.v_arr);

      memset(&to.Item, 0, sizeof(to.Item));
      to.Size = from.Size;
      to.Type = from.Type;
      to.Item.v_arr = allocArray<T>(from.Size);

      T* toVect = reinterpret_cast<T *>(to.Item.v_arr);
      T* fromVect = reinterpret_cast<T *>(from.Item.v_arr);

      for (int i = 0; i < to.Size; ++i)
        toVect[i] = fromVect[i];
    }

    template<class T>
    void AllocAndCopy(adapter::Variant::Data& d, std::vector<T> const& v)
    {
      d.Size = static_cast<int>(v.size());
      if (d.Size > 0)
      {
        if (IsArray() && d.Item.v_arr)
          freeArray<T>(d.Item.v_arr);

        memset(&d.Item, 0, sizeof(d.Item));
        d.Item.v_arr = allocArray<T>(d.Size);

        T* arr = reinterpret_cast<T *>(d.Item.v_arr);
        for (int i = 0; i < d.Size; ++i)
          arr[i] = v[i];
      }
      else
      {
        d.Item.v_arr = NULL;
      }
    }
  };
}

inline std::ostream& operator << (std::ostream& os, adapter::Variant const& v)
{
  os << v.ToString();
  return os;
}
