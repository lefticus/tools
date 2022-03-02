#include <catch2/catch.hpp>
#include <lefticus/tools/consteval_invoke.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


constexpr unsigned int Factorial(unsigned int number)// NOLINT(misc-no-recursion)
{
  return number <= 1 ? number : Factorial(number - 1) * number;
}


TEST_CASE("[consteval_invoke] works")
{
  STATIC_REQUIRE(lefticus::tools::consteval_invoke(Factorial, 10) == 3628800);// NOLINT
}

TEST_CASE("[consteval_invoke] produces a constant expression")
{
  const auto value = lefticus::tools::consteval_invoke(Factorial, 3);// NOLINT
  STATIC_REQUIRE(value == 6);// NOLINT
}
