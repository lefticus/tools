#include <catch2/catch.hpp>
#include <lefticus/tools/curry.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("curry lambda")
{
  CONSTEXPR auto func = [](int x, int y, int z) { return x + y + z; };

  STATIC_REQUIRE(lefticus::tools::curry(func, 1, 2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func)(1, 2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func, 1)(2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func, 1, 2)(3) == 6);
}
