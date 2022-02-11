#include <catch2/catch.hpp>
#include <lefticus/tools/consteval_invoke.hpp>
#include <lefticus/tools/static_views.hpp>

constexpr unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}


TEST_CASE("consteval_invoke works")
{
  STATIC_REQUIRE(lefticus::tools::consteval_invoke(Factorial, 10) == 3628800); // NOLINT
}

TEST_CASE("consteval_invoke produces a constant expression")
{
  const auto value = lefticus::tools::consteval_invoke(Factorial, 3); // NOLINT
  STATIC_REQUIRE(value == 6); // NOLINT
}

constexpr std::string make_string()
{
  std::string result;
  result = "Hello ";
  result += "World";
  result += " Test Long String";
  return result;
}


TEST_CASE("to_string_view produces a std::string_view")
{
  constexpr static auto result = lefticus::tools::to_string_view([]() { return make_string(); });
  static_assert(std::is_same_v<decltype(result), const std::string_view>);
  STATIC_REQUIRE(result == "Hello World Test Long String");
}

constexpr std::vector<double> make_vector()
{
  std::vector result{1.2, 2.4};
  result.push_back(3.6);
  return result;
}

TEST_CASE("to_span produces an std::span")
{
  constexpr static auto result = lefticus::tools::to_span([]() { return make_vector(); });
  static_assert(std::is_same_v<decltype(result), const std::span<const double>>);
  STATIC_REQUIRE(result[0] == 1.2);
  STATIC_REQUIRE(result.size() == 3);

}

