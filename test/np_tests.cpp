#include <catch2/catch.hpp>
#include <lefticus/tools/non_promoting_ints.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("[int_np] UDLs work")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto npint1 = 1_np8;
  static_assert(std::is_same_v<decltype(npint1), const lefticus::tools::int_np<std::int8_t>>);
}


TEST_CASE("[int_np] npint + int = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto npint1 = 1_np8 + std::int8_t{ 2 };
  static_assert(std::is_same_v<decltype(npint1), const lefticus::tools::int_np<std::int8_t>>);
}

TEST_CASE("[int_np] int + npint = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto npint1 = std::int8_t{ 2 } + 1_np8;
  static_assert(std::is_same_v<decltype(npint1), const lefticus::tools::int_np<std::int8_t>>);
}

TEST_CASE("[int_np] (npint << int) = npint")
{
  using namespace lefticus::tools::literals;

  CONSTEXPR const auto npint1 = 1_np8 << 2;
  static_assert(std::is_same_v<decltype(npint1), const lefticus::tools::int_np<std::int8_t>>);
}


TEST_CASE("[int_np] (2_np8 << 3) == 26")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE((2_np8 << 3) == 16_np8);
}


TEST_CASE("[int_np] can compare to underlying type")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE(2_np8 == static_cast<std::int8_t>(2));
}


TEST_CASE("[int_np] is only constructible from underlying type")
{
  using namespace lefticus::tools::literals;

  STATIC_REQUIRE(std::is_constructible_v<lefticus::tools::int_np8_t, std::int8_t>);
  STATIC_REQUIRE(!std::is_constructible_v<lefticus::tools::int_np8_t, std::uint8_t>);
}
