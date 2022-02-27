#include <array>
#include <catch2/catch.hpp>
#include <lefticus/tools/lambda_coroutines.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


CONSTEXPR std::array<unsigned long long, 10> get_first_10()
{
  auto fib = [state = 0, fib_2 = 0ULL, fib_1 = 1ULL]() mutable -> std::optional<unsigned long long> {
    lambda_co_begin(state);

    lambda_co_yield(0);
    lambda_co_yield(1);

    while (fib_1 < std::numeric_limits<decltype(fib_1)>::max() / 2) {
      fib_2 = std::exchange(fib_1, fib_2 + fib_1);
      lambda_co_yield(fib_1);
    }

    lambda_co_return({});
  };

  const auto _1 = fib().value();
  const auto _2 = fib().value();
  const auto _3 = fib().value();
  const auto _4 = fib().value();
  const auto _5 = fib().value();
  const auto _6 = fib().value();
  const auto _7 = fib().value();
  const auto _8 = fib().value();
  const auto _9 = fib().value();
  const auto _10 = fib().value();


  return { _1, _2, _3, _4, _5, _6, _7, _8, _9, _10 };
}

#if (defined(_MSC_VER) && defined(CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)) || !defined(_MSC_VER)
TEST_CASE("lambda_coroutines are constexpr capable", "[constexpr]") { STATIC_REQUIRE(get_first_10()[4] == 3ULL); }
#else
#pragma message("Visual Studio's constexpr engine is not capable of handling the lambda coroutines")
#endif
