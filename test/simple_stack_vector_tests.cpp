#include <catch2/catch.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


TEST_CASE("[simple_stack_vector] starts empty")
{
  STATIC_REQUIRE(lefticus::tools::simple_stack_vector<int, 10>{}.empty() == true);
}


TEST_CASE("[simple_stack_vector] can be created from initializer_list")
{
  CONSTEXPR auto vec = lefticus::tools::simple_stack_vector<char, 10>{ 'a', 'b', 'c', 'd' };
  STATIC_REQUIRE(vec.size() == 4);
}

TEST_CASE("[simple_stack_vector] const reverse iterators work")
{
  const auto reverse = [](const auto &input) {
    lefticus::tools::simple_stack_vector<int, 10> output;
    for (auto itr = input.rbegin(); itr != input.rend(); ++itr) { output.push_back(*itr); }
    return output;
  };

  CONSTEXPR auto reversed_1 = reverse(lefticus::tools::simple_stack_vector<int, 10>{});
  STATIC_REQUIRE(reversed_1 == lefticus::tools::simple_stack_vector<int, 1>{});// NOLINT (check for empty)
  STATIC_REQUIRE(reversed_1 == lefticus::tools::simple_stack_vector<int, 0>{});// NOLINT (check for empty)
  STATIC_REQUIRE(reversed_1.empty());

  CONSTEXPR auto reversed_2 = reverse(lefticus::tools::simple_stack_vector<int, 10>{ 1 });
  STATIC_REQUIRE(reversed_2 == lefticus::tools::simple_stack_vector<int, 1>{ 1 });

  CONSTEXPR auto reversed_3 = reverse(lefticus::tools::simple_stack_vector<int, 10>{ 1, 2, 3, 4 });
  STATIC_REQUIRE(reversed_3 == lefticus::tools::simple_stack_vector<int, 10>{ 4, 3, 2, 1 });
}

TEST_CASE("[simple_stack_vector] c-reverse iterators work")
{
  const auto reverse = [](const auto &input) {
    lefticus::tools::simple_stack_vector<int, 10> output;
    for (auto itr = input.crbegin(); itr != input.crend(); ++itr) { output.push_back(*itr); }
    return output;
  };

  CONSTEXPR auto reversed_1 = reverse(lefticus::tools::simple_stack_vector<int, 10>{});
  STATIC_REQUIRE(reversed_1 == lefticus::tools::simple_stack_vector<int, 0>{});// NOLINT (check for empty)
  STATIC_REQUIRE(reversed_1.empty());

  CONSTEXPR auto reversed_2 = reverse(lefticus::tools::simple_stack_vector<int, 10>{ 1 });
  STATIC_REQUIRE(reversed_2 == lefticus::tools::simple_stack_vector<int, 1>{ 1 });

  CONSTEXPR auto reversed_3 = reverse(lefticus::tools::simple_stack_vector<int, 10>{ 1, 2, 3, 4 });
  STATIC_REQUIRE(reversed_3 == lefticus::tools::simple_stack_vector<int, 10>{ 4, 3, 2, 1 });
}


TEST_CASE("[simple_stack_vector] reverse iterators work")
{
  const auto increment = [](auto input) {
    for (auto itr = input.rbegin(); itr != input.rend(); ++itr) { ++(*itr); }
    return input;
  };

  CONSTEXPR auto incrementd_1 = increment(lefticus::tools::simple_stack_vector<int, 10>{});
  STATIC_REQUIRE(incrementd_1 == lefticus::tools::simple_stack_vector<int, 0>{});// NOLINT (check for empty)
  STATIC_REQUIRE(incrementd_1.empty());// NOLINT (check for empty)

  CONSTEXPR auto incrementd_2 = increment(lefticus::tools::simple_stack_vector<int, 10>{ 1 });
  STATIC_REQUIRE(incrementd_2 == lefticus::tools::simple_stack_vector<int, 1>{ 2 });

  CONSTEXPR auto incrementd_3 = increment(lefticus::tools::simple_stack_vector<int, 10>{ 1, 2, 3, 4 });
  STATIC_REQUIRE(incrementd_3 == lefticus::tools::simple_stack_vector<int, 4>{ 2, 3, 4, 5 });
}


TEST_CASE("[simple_stack_vector] push_back works")
{
  const auto create = []() {
    lefticus::tools::simple_stack_vector<int, 10> vec;
    vec.push_back(5);
    vec.push_back(10);
    return vec;
  };

  CONSTEXPR auto vec = create();
  STATIC_REQUIRE(vec.size() == 2);
  STATIC_REQUIRE(vec[0] == 5);
  STATIC_REQUIRE(vec[1] == 10);
}

TEST_CASE("[simple_stack_vector] emplace_back works")
{
  const auto create = []() {
    lefticus::tools::simple_stack_vector<std::pair<int, int>, 10> vec;
    vec.emplace_back(1, 2);
    vec.emplace_back(3, 4).second = 5;
    return vec;
  };

  CONSTEXPR auto vec = create();
  STATIC_REQUIRE(vec.size() == 2);
  STATIC_REQUIRE(vec[0] == std::pair<int, int>{ 1, 2 });
  STATIC_REQUIRE(vec[1] == std::pair<int, int>{ 3, 5 });
}

TEST_CASE("[simple_stack_vector] end / cend work")
{
  const auto create = []() {
    lefticus::tools::simple_stack_vector<int, 10> vec{ 1, 2, 3 };
    return vec;
  };

  // this should cover the const/constexpr/and non-const cases
  CONSTEXPR auto vec = create();// imlicit const in the CONSTEXPR build, non-const in the non-CONSTEXPR build
  STATIC_REQUIRE(vec.size() == 3);
  STATIC_REQUIRE(std::distance(vec.begin(), vec.end()) == 3);

  // but we still need a test to cover the non-const constexpr use case of begin()/end() and specifically calls to
  // cbegin/cend
  STATIC_REQUIRE(std::distance(vec.cbegin(), vec.cend()) == 3);


  const auto get_size_from_iterators = []() {
    lefticus::tools::simple_stack_vector<int, 10> ret{ 1, 2, 3 };
    // these will be non-const access to begin and end, even in the constexpr tests
    return std::distance(ret.begin(), ret.end());
  };

  STATIC_REQUIRE(get_size_from_iterators() == 3);
}
