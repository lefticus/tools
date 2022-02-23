#include <catch2/catch.hpp>
#include <lefticus/tools/consteval_invoke.hpp>
#include <lefticus/tools/static_views.hpp>
#include <lefticus/tools/non_promoting_ints.hpp>
#include <lefticus/tools/lambda_coroutines.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>
#include <lefticus/tools/simple_stack_string.hpp>
#include <lefticus/tools/curry.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
//NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


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
  std::vector result{1.2, 2.4};
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



TEST_CASE("npint UDLs work") {
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto i1 = 1_np8;
  static_assert(std::is_same_v < decltype(i1), const lefticus::tools::int_np<std::int8_t>>);
}


TEST_CASE("npint + int = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto i1 = 1_np8 + std::int8_t{ 2 };
  static_assert(std::is_same_v<decltype(i1), const lefticus::tools::int_np<std::int8_t>>);
}

TEST_CASE("int + npint = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto i1 = std::int8_t{ 2 } + 1_np8;
  static_assert(std::is_same_v<decltype(i1), const lefticus::tools::int_np<std::int8_t>>);
}

TEST_CASE("npint << int = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto i1 = 1_np8 << 2;
  static_assert(std::is_same_v<decltype(i1), const lefticus::tools::int_np<std::int8_t>>);
}



TEST_CASE("2_np8 << 3 == 26")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE((2_np8 << 3) == 16_np8);
}


TEST_CASE("can compare np_int to underlying type")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE(2_np8 == static_cast<std::int8_t>(2));
}


TEST_CASE("np_int is only constructible from underlying type")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE(std::is_constructible_v<lefticus::tools::int_np8_t, std::int8_t>);
  STATIC_REQUIRE(!std::is_constructible_v<lefticus::tools::int_np8_t, std::uint8_t>);
}



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
TEST_CASE("lambda_coroutines are constexpr capable", "[constexpr]")
{ 
  STATIC_REQUIRE(get_first_10()[4] == 3ULL);
}
#else
#pragma message("Visual Studio's constexpr engine is not capable of handling the lambda coroutines")
#endif

TEST_CASE("simple_stack_vector starts empty") { STATIC_REQUIRE(lefticus::tools::simple_stack_vector<int, 10>{}.empty() == true); }


TEST_CASE("curry lambda")
{
  CONSTEXPR auto func = [](int x, int y, int z) { return x + y + z; };

  STATIC_REQUIRE(lefticus::tools::curry(func, 1, 2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func)(1, 2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func, 1)(2, 3) == 6);
  STATIC_REQUIRE(lefticus::tools::curry(func, 1, 2)(3) == 6);
}

