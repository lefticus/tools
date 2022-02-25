#include <catch2/catch.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
//NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("simple_stack_vector starts empty") { STATIC_REQUIRE(lefticus::tools::simple_stack_vector<int, 10>{}.empty() == true); }



