#pragma once

namespace adapter 
{
  enum class TypeId
  {
    Null,
    Boolean,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Double,
    String,

    // array types
    BooleanArray,
    UInt8Array,
    Int16Array,
    UInt16Array,
    Int32Array,
    UInt32Array,
    Int64Array,
    UInt64Array,
    DoubleArray,
    StringArray
  };

  inline char const* TypeIdToString(TypeId id)
  {
    char const* s = "unknown";
    #define TYPEID_CASE(T) case TypeId::T: s = #T; break
    switch (id)
    {
      TYPEID_CASE(Null);
      TYPEID_CASE(Boolean);
      TYPEID_CASE(UInt8);
      TYPEID_CASE(Int16);
      TYPEID_CASE(UInt16);
      TYPEID_CASE(Int32);
      TYPEID_CASE(UInt32);
      TYPEID_CASE(Int64);
      TYPEID_CASE(UInt64);
      TYPEID_CASE(Double);
      TYPEID_CASE(String);
      TYPEID_CASE(BooleanArray);
      TYPEID_CASE(UInt8Array);
      TYPEID_CASE(Int16Array);
      TYPEID_CASE(UInt16Array);
      TYPEID_CASE(Int32Array);
      TYPEID_CASE(UInt32Array);
      TYPEID_CASE(Int64Array);
      TYPEID_CASE(UInt64Array);
      TYPEID_CASE(DoubleArray);
      TYPEID_CASE(StringArray);
    }
    #undef TYPEID_CASE
    return s;
  }
}
