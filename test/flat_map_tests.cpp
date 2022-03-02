#include <catch2/catch.hpp>
#include <lefticus/tools/simple_stack_flat_map.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("[simple_stack_flat_map] starts empty")
{
  CONSTEXPR auto map = lefticus::tools::simple_stack_flat_map<int, int, 5>{};

  STATIC_REQUIRE(map.empty());
  STATIC_REQUIRE(map.size() == 0); // NOLINT use empty()
  STATIC_REQUIRE(map.begin() == map.end());
  STATIC_REQUIRE(map.max_size() == 5);
}


TEST_CASE("[simple_stack_flat_map] can be initialized")
{
  CONSTEXPR auto map = lefticus::tools::simple_stack_flat_map<int, int, 5>{{1,2}, {3,4}};

  STATIC_REQUIRE(!map.empty());
  STATIC_REQUIRE(map.size() == 2);
  STATIC_REQUIRE(map.begin() != map.end());
  STATIC_REQUIRE(map.max_size() == 5);
  STATIC_REQUIRE(map.at(1) == 2);
  STATIC_REQUIRE(map.at(3) == 4);
}


TEST_CASE("[simple_stack_flat_map] is constexpr usable")
{
  const auto make_map = []() {
    lefticus::tools::simple_stack_flat_map<int, int, 5> m;
    m[1] = 2;
    m[3] = 4;
    return m;
  };

  CONSTEXPR auto map = make_map();

  STATIC_REQUIRE(!map.empty());
  STATIC_REQUIRE(map.size() == 2);
  STATIC_REQUIRE(map.begin() != map.end());
  STATIC_REQUIRE(map.max_size() == 5);
  STATIC_REQUIRE(map.at(1) == 2);
  STATIC_REQUIRE(map.at(3) == 4);
}
