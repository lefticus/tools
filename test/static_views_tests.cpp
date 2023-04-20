#include <catch2/catch.hpp>
#include <lefticus/tools/flat_map.hpp>
#include <lefticus/tools/simple_stack_flat_map.hpp>
#include <lefticus/tools/simple_stack_string.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>
#include <lefticus/tools/static_views.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif


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
  std::vector result{ 1.2, 2.4 };// NOLINT Magic Number
  result.push_back(3.6);// NOLINT Magic Number
  return result;
}

TEST_CASE("to_span produces an std::span from std::vector")
{
  constexpr static auto result = lefticus::tools::to_span([]() { return make_vector(); });
  static_assert(std::is_same_v<decltype(result), const std::span<const double>>);
  STATIC_REQUIRE(result[0] == 1.2);// NOLINT Magic Number
  STATIC_REQUIRE(result.size() == 3);
}
#endif

constexpr auto make_vector_like()
{
  lefticus::tools::simple_stack_vector<double, 10> result{ 1.2, 2.4 };// NOLINT Magic Number
  result.push_back(3.6);// NOLINT Magic Number
  return result;
}

TEST_CASE("[to_span] produces an std::span from simple_stack_vector")
{
  CONSTEXPR const auto result = lefticus::tools::to_span([]() { return make_vector_like(); });
  static_assert(std::is_same_v<decltype(result), const std::span<const double>>);
  STATIC_REQUIRE(result[0] == 1.2);// NOLINT Magic Number
  STATIC_REQUIRE(result.size() == 3);
}

TEST_CASE("[to_right_sized_array] produces an array of the correct size")
{
  CONSTEXPR auto result = lefticus::tools::to_right_sized_array([]() { return make_vector_like(); });
  STATIC_REQUIRE(result[0] == 1.2);// NOLINT Magic Number
  STATIC_REQUIRE(result.size() == 3);
  STATIC_REQUIRE(sizeof(result) == 3 * sizeof(double));
}

constexpr auto make_string_like()
{
  lefticus::tools::simple_stack_string<10> result{ "Hello" };
  result.push_back('a');
  return result;
}

TEST_CASE("[to_string_view] produces an std::string_view from simple_stack_string")
{
  CONSTEXPR const auto result = lefticus::tools::to_string_view([]() { return make_string_like(); });
  static_assert(std::is_same_v<decltype(result), const std::string_view>);
  STATIC_REQUIRE(result[0] == 'H');
  STATIC_REQUIRE(result.size() == 6);// NOLINT Magic Number
}


TEST_CASE("[resize] can right-size a container level 1")// NOLINT (cognitive complexity)
{
  constexpr lefticus::tools::simple_stack_vector<int, 16> vec{ 1, 2, 3, 4, 5 };

  STATIC_REQUIRE(vec.size() == 5);// NOLINT Magic Number
  STATIC_REQUIRE(vec.max_size() == 16);// NOLINT Magic Number

  constexpr auto bigger = lefticus::tools::stackify<32>(vec);

  // stackify should never grow something that's already stack-based
  STATIC_REQUIRE(bigger.size() == 5);// NOLINT Magic Number
  STATIC_REQUIRE(bigger.max_size() == 16);// NOLINT Magic Number

  constexpr auto max_sizes = lefticus::tools::max_element_size(bigger);

  STATIC_REQUIRE(max_sizes.first == 5);// NOLINT Magic Number
  STATIC_REQUIRE(max_sizes.second == 1);

  constexpr auto minimized = lefticus::tools::resize<max_sizes>(bigger);

  STATIC_REQUIRE(minimized.capacity() == 5);// NOLINT Magic Number

  STATIC_REQUIRE(sizeof(minimized) < sizeof(bigger));
}

TEST_CASE("[resize] can right-size a container level 2")// NOLINT (cognitive complexity)
{
  constexpr lefticus::tools::simple_stack_vector<lefticus::tools::simple_stack_string<16>, 16> vec{
    // NOLINT Magic Number
    lefticus::tools::simple_stack_string<16>("Hello"),// NOLINT Magic Number
    lefticus::tools::simple_stack_string<16>("long string")// NOLINT Magic Number
  };

  constexpr auto max_sizes = lefticus::tools::max_element_size(vec);

  STATIC_REQUIRE(max_sizes.first == 2);
  STATIC_REQUIRE(max_sizes.second == 11);// NOLINT Magic Number

  constexpr auto minimized = lefticus::tools::resize<max_sizes>(vec);

  STATIC_REQUIRE(minimized.capacity() == 2);

  STATIC_REQUIRE(sizeof(minimized) < sizeof(vec));
}

template<typename T> using vector = lefticus::tools::simple_stack_vector<T, 16>;// NOLINT Magic Number
template<typename Key, typename Value>
using map = lefticus::tools::simple_stack_flat_map<Key, Value, 16>;// NOLINT Magic Number
using string = lefticus::tools::simple_stack_string<16>;// NOLINT Magic Number

TEST_CASE("[minimized_stackify] works")// NOLINT (cognitive complexity)
{
  using namespace std::string_view_literals;
  const auto make_data = []() {
    map<string, map<string, vector<int>>> data;
    data["hello"]["world"].push_back(42);// NOLINT Magic Number
    data["hello"]["jason"].push_back(72);// NOLINT Magic Number
    data["test"]["data"].push_back(84);// NOLINT Magic Number
    return data;
  };

  CONSTEXPR auto minimized = lefticus::tools::minimized_stackify<32>(make_data);// NOLINT Magic Number

  STATIC_REQUIRE(minimized.max_size() == 2);
  STATIC_REQUIRE(minimized.at("hello").max_size() == 2);
  STATIC_REQUIRE(minimized.at("hello").at("world").capacity() == 1);
}


#if __cpp_lib_constexpr_string >= 201907L && __cpp_lib_constexpr_vector >= 201907L

#if (defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 0) || !defined(_ITERATOR_DEBUG_LEVEL)

TEST_CASE("[minimized_stackify] works with std::vector, std::string")// NOLINT (cognitive complexity)
{
  using namespace std::string_view_literals;
  const auto make_data = []() {
    lefticus::tools::flat_map<std::string, lefticus::tools::flat_map<std::string, std::vector<int>>> data;
    data["hello"]["world"].push_back(42);// NOLINT Magic Number
    data["hello"]["jason"].push_back(72);// NOLINT Magic Number
    data["test"]["data"].push_back(84);// NOLINT Magic Number
    return data;
  };

  CONSTEXPR auto minimized = lefticus::tools::minimized_stackify<32>(make_data);// NOLINT Magic Number

  STATIC_REQUIRE(minimized.max_size() == 2);
  STATIC_REQUIRE(minimized.at("hello").max_size() == 2);
  STATIC_REQUIRE(minimized.at("hello").at("world").capacity() == 1);
}

#else

#pragma message("Visual Studio's debug iterators are not fully constexpr capable")

#endif

#endif
