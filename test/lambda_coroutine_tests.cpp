#include <array>
#include <catch2/catch.hpp>
#include <lefticus/tools/lambda_coroutines.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


CONSTEXPR std::array<unsigned long long, 10> get_first_10() // NOLINT magic number
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

  const auto val_1 = fib().value(); // NOLINT Unchecked access
  const auto val_2 = fib().value(); // NOLINT Unchecked access
  const auto val_3 = fib().value(); // NOLINT Unchecked access
  const auto val_4 = fib().value(); // NOLINT Unchecked access
  const auto val_5 = fib().value(); // NOLINT Unchecked access
  const auto val_6 = fib().value(); // NOLINT Unchecked access
  const auto val_7 = fib().value(); // NOLINT Unchecked access
  const auto val_8 = fib().value(); // NOLINT Unchecked access
  const auto val_9 = fib().value(); // NOLINT Unchecked access
  const auto val_10 = fib().value(); // NOLINT Unchecked access


  return { val_1, val_2, val_3, val_4, val_5, val_6, val_7, val_8, val_9, val_10 };
}

#if (defined(_MSC_VER) && defined(CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)) || !defined(_MSC_VER)
TEST_CASE("[lambda_coroutines] is constexpr capable") { STATIC_REQUIRE(get_first_10()[4] == 3ULL); }
#else
#pragma message("Visual Studio's constexpr engine is not capable of handling the lambda coroutines")
#endif
