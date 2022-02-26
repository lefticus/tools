#include <catch2/catch.hpp>
#include <lefticus/tools/simple_stack_string.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("simple_stack_string starts empty")
{
  STATIC_REQUIRE(lefticus::tools::simple_stack_string<10>{}.empty() == true);
}

TEST_CASE("basic_simple_stack_string can be used")
{
  CONSTEXPR bool type_match =
    std::is_same_v<typename lefticus::tools::basic_simple_stack_string<wchar_t, 10>::traits_type,
      std::char_traits<wchar_t>>;
  STATIC_REQUIRE(type_match == true);
}


TEST_CASE("simple_stack_string can be created from initializer_list")
{
  CONSTEXPR auto str = lefticus::tools::simple_stack_string<10>{ 'a', 'b', 'c', 'd' };
  STATIC_REQUIRE(str.size() == 4);
}


TEST_CASE("simple_stack_string can be created from const char *")
{
  CONSTEXPR auto str = lefticus::tools::simple_stack_string<10>{ "abcd" };
  STATIC_REQUIRE(str.size() == 4);
}

TEST_CASE("simple_stack_string can be created from string_view")
{
  CONSTEXPR auto sv = std::string_view{ "abcd" };
  CONSTEXPR auto str = lefticus::tools::simple_stack_string<10>{ sv };
  STATIC_REQUIRE(str.size() == 4);
}


TEST_CASE("simple_stack_string const reverse iterators work")
{
  const auto reverse = [](const auto &input) {
    lefticus::tools::simple_stack_string<10> output;
    for (auto itr = input.rbegin(); itr != input.rend(); ++itr) { output.push_back(*itr); }
    return output;
  };

  CONSTEXPR auto reversed_1 = reverse(lefticus::tools::simple_stack_string<10>{ "" });
  STATIC_REQUIRE(reversed_1 == "");

  CONSTEXPR auto reversed_2 = reverse(lefticus::tools::simple_stack_string<10>{ "a" });
  STATIC_REQUIRE(reversed_2 == "a");

  CONSTEXPR auto reversed_3 = reverse(lefticus::tools::simple_stack_string<10>{ "abcd" });
  STATIC_REQUIRE(reversed_3 == "dcba");
}

TEST_CASE("simple_stack_string c-reverse iterators work")
{
  const auto reverse = [](const auto &input) {
    lefticus::tools::simple_stack_string<10> output;
    for (auto itr = input.crbegin(); itr != input.crend(); ++itr) { output.push_back(*itr); }
    return output;
  };

  CONSTEXPR auto reversed_1 = reverse(lefticus::tools::simple_stack_string<10>{ "" });
  STATIC_REQUIRE(reversed_1 == "");

  CONSTEXPR auto reversed_2 = reverse(lefticus::tools::simple_stack_string<10>{ "a" });
  STATIC_REQUIRE(reversed_2 == "a");

  CONSTEXPR auto reversed_3 = reverse(lefticus::tools::simple_stack_string<10>{ "abcd" });
  STATIC_REQUIRE(reversed_3 == "dcba");
}


TEST_CASE("simple_stack_string reverse iterators work")
{
  const auto increment = [](auto input) {
    lefticus::tools::simple_stack_string<10> output;
    for (auto itr = input.rbegin(); itr != input.rend(); ++itr) { ++(*itr); }
    return input;
  };

  CONSTEXPR auto incrementd_1 = increment(lefticus::tools::simple_stack_string<10>{ "" });
  STATIC_REQUIRE(incrementd_1 == "");

  CONSTEXPR auto incrementd_2 = increment(lefticus::tools::simple_stack_string<10>{ "a" });
  STATIC_REQUIRE(incrementd_2 == "b");

  CONSTEXPR auto incrementd_3 = increment(lefticus::tools::simple_stack_string<10>{ "abcd" });
  STATIC_REQUIRE(incrementd_3 == "bcde");
}

TEST_CASE("simple_stack_string + char string literal")
{
  CONSTEXPR lefticus::tools::simple_stack_string<10> str{ "Hello" };
  CONSTEXPR auto hello_world = str + " World";
  STATIC_REQUIRE(hello_world.size() == 11);
  STATIC_REQUIRE(hello_world.capacity() == 15);
}


TEST_CASE("char string literal + simple_stack_string")
{
  CONSTEXPR lefticus::tools::simple_stack_string<10> str{ " World" };
  CONSTEXPR auto hello_world = "Hello" + str;
  STATIC_REQUIRE(hello_world.size() == 11);
  STATIC_REQUIRE(hello_world.capacity() == 14);
}

TEST_CASE("simple_stack_string + simple_stack_string")
{
  CONSTEXPR lefticus::tools::simple_stack_string<10> str1{ "Hello" };
  CONSTEXPR lefticus::tools::simple_stack_string<10> str2{ " World" };
  CONSTEXPR auto hello_world = str1 + str2;
  STATIC_REQUIRE(hello_world.size() == 11);
  STATIC_REQUIRE(hello_world.capacity() == 18);
}

TEST_CASE("basic_simple_stack_string CTAD")
{
  CONSTEXPR lefticus::tools::basic_simple_stack_string str1{ "Hello" };
  STATIC_REQUIRE(str1.size() == 5);
  STATIC_REQUIRE(str1.capacity() == 5);
  STATIC_REQUIRE(decltype(str1)::total_capacity == 6);
}


TEST_CASE("simple_stack_string to_sss")
{
  using namespace lefticus::tools::literals;
  CONSTEXPR auto str1 = to_sss("Hello");
  STATIC_REQUIRE(str1.size() == 5);
  STATIC_REQUIRE(str1.capacity() == 5);
  STATIC_REQUIRE(decltype(str1)::total_capacity == 6);
}

TEST_CASE("simple_stack_string to_sss + to_sss")
{
  using namespace lefticus::tools::literals;
  CONSTEXPR auto str1 = to_sss("Hello") + to_sss(" World");
  STATIC_REQUIRE(str1.size() == 11);
  STATIC_REQUIRE(str1.capacity() == 11);
  STATIC_REQUIRE(decltype(str1)::total_capacity == 12);
}


TEST_CASE("simple_stack_string simple_stack_string == string_view, sv == sss")
{
  using namespace lefticus::tools::literals;
  STATIC_REQUIRE(to_sss("Hello") == std::string_view{ "Hello" });
  STATIC_REQUIRE(std::string_view{ "Hello" } == to_sss("Hello"));
}

TEST_CASE("simple_stack_string simple_stack_string == const char *, const char * == sss")
{
  using namespace lefticus::tools::literals;
  STATIC_REQUIRE(to_sss("Hello") == "Hello");
  STATIC_REQUIRE("Hello" == to_sss("Hello"));
}
