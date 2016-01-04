
#include <gtest/gtest.h>
#include <Variant.h>

template<class T>
std::vector<T> makeVect(int n, int start = 0)
{
  std::vector<T> v;
  for (int i = start; i < start + n; ++i)
    v.push_back(i);
  return v;
}

bool TestVariant_Boolean1()
{
  common::Variant v(true);
  return v.ToBoolean();
}

bool TestVariant_Boolean2()
{
  common::Variant v(false);
  return v.ToBoolean();
}

uint8_t TestVariant_UInt8(uint8_t n)
{
  common::Variant v(n);
  return v.ToUInt8();
}

std::string TestVariant_StdString(std::string const& s)
{
  common::Variant v(s);
  return v.ToString();
}

TEST(VariantTest, Boolean) {
  EXPECT_TRUE(TestVariant_Boolean1());
  EXPECT_FALSE(TestVariant_Boolean2());
}

TEST(VariantTest, UInt8) {
  EXPECT_EQ(0, TestVariant_UInt8(0));
  EXPECT_EQ(10, TestVariant_UInt8(10));
  EXPECT_EQ(100, TestVariant_UInt8(100));
  EXPECT_EQ(200, TestVariant_UInt8(200));
  EXPECT_EQ(std::numeric_limits<uint8_t>::max(), TestVariant_UInt8(std::numeric_limits<uint8_t>::max()));

  for (uint8_t n = 0; n < std::numeric_limits<uint8_t>::max(); ++n)
  {
    EXPECT_EQ(n, TestVariant_UInt8(n));
  }
}

TEST(Variant, UInt8Array) {
  std::vector<uint8_t> arr = makeVect<uint8_t>(10);

  common::Variant v1(arr);
  EXPECT_EQ(v1.ToUInt8Array(), arr);
  EXPECT_EQ(v1.Length(), 10);
  EXPECT_TRUE(v1.IsArray());

  arr.clear();
  arr.push_back(10);
  arr.push_back(20);
  v1 = arr;
  EXPECT_TRUE(v1.IsArray());
  EXPECT_EQ(v1.Length(), 2);
  std::vector<uint8_t> arr2 = v1.ToUInt8Array();

  EXPECT_EQ(arr.size(), arr2.size());
  EXPECT_EQ(arr[0], arr2[0]);
  EXPECT_EQ(arr[1], arr2[1]);

  EXPECT_EQ(std::string("[10,20]"), v1.ToString());
}

TEST(Variant, StringArray)
{
  std::vector<std::string> a1;
  a1.push_back("a");
  a1.push_back("b");

  common::Variant v1(a1);
  common::Variant v2 = v1;

  EXPECT_TRUE(v1.IsArray());
  EXPECT_EQ(v1.Length(), 2);
}

TEST(VariantTest, String) {
  EXPECT_EQ(std::string("hello world"), TestVariant_StdString("hello world"));
  EXPECT_EQ(std::string(""), TestVariant_StdString(""));
  EXPECT_EQ(std::string(), TestVariant_StdString(""));
}

TEST(VariantTest, ToString)
{
  common::Variant v1;
  v1 = (uint8_t) 234;
  EXPECT_EQ("234", v1.ToString());

  v1 = (int) 1234567;
  EXPECT_EQ("1234567", v1.ToString());

  v1 = (uint64_t) 18446744073709551615ul;;
  EXPECT_EQ("18446744073709551615", v1.ToString());
}

TEST(VariantTest, StringCopyAssign) {
  common::Variant v1;
  common::Variant v2;

  v1 = "asdf123";
  v2 = "asdf123";
  EXPECT_EQ(v1.ToString(), v2.ToString());

  v2 = "";
  EXPECT_NE(v1.ToString(), v2.ToString());

  v1 = v2;
  EXPECT_EQ(v1.ToString(), v2.ToString());
}

TEST(Variant, Convert)
{
  common::Variant v1( (uint8_t) 12 );

  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::Boolean));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt8));

  // maybe extend Variant to do runtime checking. i.e int64_t == 10 is convertable to
  // int8_t, etc.
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt16));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt32));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt64));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::Int16));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::Int32));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::Int64));

  v1 = (uint32_t) 12345;
  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::Boolean));
  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::UInt8));
  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::UInt16));
  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::Int16));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt32));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::UInt64));
  EXPECT_FALSE(v1.CanConvert(common::Variant::DataType::Int32));
  EXPECT_TRUE(v1.CanConvert(common::Variant::DataType::Int64));
}

TEST(Variant, OperatorLessThan)
{
  common::Variant v1;
  common::Variant v2;

  EXPECT_FALSE(v1 < v2);

  v2 = (int32_t) 10;
  EXPECT_TRUE(v1 < v2);

  v1 = (int32_t) 11;
  EXPECT_TRUE(v2 < v1);

  v2 = (uint8_t) 12;
  EXPECT_TRUE(v2 < v1);

  v1 = (uint8_t) 11;
  EXPECT_TRUE(v1 < v2);

  v1 = makeVect<int16_t>(10);
  v2 = makeVect<int16_t>(10);
  EXPECT_FALSE(v1 < v2);
  EXPECT_FALSE(v2 < v1);

  v2 = makeVect<int16_t>(10, 2);
  EXPECT_TRUE(v1 < v2);
  EXPECT_FALSE(v2 < v1);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

