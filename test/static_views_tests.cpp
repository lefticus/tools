#include <catch2/catch.hpp>
#include <lefticus/tools/simple_stack_string.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>
#include <lefticus/tools/static_views.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


#if __cpp_lib_constexpr_string >= 201907L
constexpr std::string make_string()
{
  std::string result;
  result = "Hello ";
  result += "World";
  result += " Test Long String";
  return result;
}


TEST_CASE("to_string_view produces a std::string_view from std::string")
{
  constexpr static auto result = lefticus::tools::to_string_view([]() { return make_string(); });
  static_assert(std::is_same_v<decltype(result), const std::string_view>);
  STATIC_REQUIRE(result == "Hello World Test Long String");
}
#endif

#if __cpp_lib_constexpr_vector >= 201907L
constexpr std::vector<double> make_vector()
{
  std::vector result{ 1.2, 2.4 };
  result.push_back(3.6);
  return result;
}

TEST_CASE("to_span produces an std::span from std::vector")
{
  constexpr static auto result = lefticus::tools::to_span([]() { return make_vector(); });
  static_assert(std::is_same_v<decltype(result), const std::span<const double>>);
  STATIC_REQUIRE(result[0] == 1.2);
  STATIC_REQUIRE(result.size() == 3);
}
#endif

constexpr auto make_vector_like()
{
  lefticus::tools::simple_stack_vector<double, 10> result{ 1.2, 2.4 };
  result.push_back(3.6);
  return result;
}

TEST_CASE("to_span produces an std::span from simple_stack_vector")
{
  CONSTEXPR const auto result = lefticus::tools::to_span([]() { return make_vector_like(); });
  static_assert(std::is_same_v<decltype(result), const std::span<const double>>);
  STATIC_REQUIRE(result[0] == 1.2);
  STATIC_REQUIRE(result.size() == 3);
}

TEST_CASE("to_right_sized_array produces an array of the correct size")
{
  CONSTEXPR auto result = lefticus::tools::to_right_sized_array([]() { return make_vector_like(); });
  STATIC_REQUIRE(result[0] == 1.2);
  STATIC_REQUIRE(result.size() == 3);
  STATIC_REQUIRE(sizeof(result) == 3 * sizeof(double));
}

constexpr auto make_string_like()
{
  lefticus::tools::simple_stack_string<10> result{ "Hello" };
  result.push_back('a');
  return result;
}

TEST_CASE("can make simple_stack_string") { [[maybe_unused]] CONSTEXPR auto result = make_string_like(); }

TEST_CASE("to_string_view produces an std::string_view from simple_stack_string")
{
  CONSTEXPR const auto result = lefticus::tools::to_string_view([]() { return make_string_like(); });
  static_assert(std::is_same_v<decltype(result), const std::string_view>);
  STATIC_REQUIRE(result[0] == 'H');
  STATIC_REQUIRE(result.size() == 6);
}
