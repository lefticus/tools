#include <catch2/catch.hpp>


#include <array>
#include <utility>

#include <lefticus/tools/curry.hpp>
#include <lefticus/tools/flat_map.hpp>
#include <lefticus/tools/static_views.hpp>

#include <lefticus/tools/lambda_coroutines.hpp>
#include <lefticus/tools/simple_stack_flat_map.hpp>
#include <lefticus/tools/simple_stack_string.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>


TEST_CASE("[lambda_coroutines] Generate infinite sequence")// NOLINT (cognitive complexity)
{
  enum struct directions { Left, Right, Up, Down };

  auto next_direction = [state = 0]() mutable {
    lambda_co_begin(state);

    while (true) {
      lambda_co_yield(directions::Left);
      lambda_co_yield(directions::Right);
      lambda_co_yield(directions::Up);
      lambda_co_yield(directions::Down);
    }

    lambda_co_end();
  };

  REQUIRE(next_direction() == directions::Left);
  REQUIRE(next_direction() == directions::Right);
  REQUIRE(next_direction() == directions::Up);
  REQUIRE(next_direction() == directions::Down);
  REQUIRE(next_direction() == directions::Left);
  REQUIRE(next_direction() == directions::Right);
}

TEST_CASE("[lambda_coroutines] Cooperative multi tasking")// NOLINT (cognitive complexity)
{
  enum OpCodes : std::uint8_t { ADD = 0, STA = 1, NOP = 2 };
  struct Machine
  {
    std::uint8_t PC{ 0 };
    std::uint8_t A{ 0 };
    std::array<uint8_t, 256> RAM{ STA, 10, ADD, 15 }; // NOLINT Magic Number
  };

  Machine machine;

  auto CPU = [state = 0, &machine, operation = OpCodes::NOP]() mutable {
    lambda_co_begin(state);

    while (true) {
      operation = static_cast<OpCodes>(machine.RAM.at(machine.PC));
      ++machine.PC;
      if (operation == OpCodes::STA) {
        lambda_co_yield();
        machine.A = machine.RAM.at(machine.PC++);
        lambda_co_yield();
      } else if (operation == OpCodes::ADD) {
        lambda_co_yield();
        machine.A += machine.RAM.at(machine.PC++);
        lambda_co_yield();
      } else if (operation == OpCodes::NOP) {
        lambda_co_yield();
      };
    }

    lambda_co_end();
  };

  CPU();
  REQUIRE(machine.PC == 1);
  REQUIRE(machine.A == 0);
  CPU();
  REQUIRE(machine.PC == 2); // NOLINT Magic Number
  REQUIRE(machine.A == 10); // NOLINT Magic Number
  CPU();
  REQUIRE(machine.PC == 3); // NOLINT Magic Number
  REQUIRE(machine.A == 10); // NOLINT Magic Number
  CPU();
  REQUIRE(machine.PC == 4); // NOLINT Magic Number
  REQUIRE(machine.A == 25); // NOLINT Magic Number
}


struct Data
{
  std::string value;
};

TEST_CASE("[curry] member object pointer w/return reference forwarding")
{
  Data data;
  auto member = lefticus::tools::curry(&Data::value);

  member(&data) = "Hello World";

  REQUIRE(data.value == "Hello World");

  std::string &value = member(&data);

  value = "A long string to test";

  REQUIRE(member(&data) == "A long string to test");
}


TEST_CASE("[simple_stack_string] simple_stack_string == std::string, std::string == sss")
{
  using namespace lefticus::tools::literals;
  REQUIRE(to_sss("Hello") == std::string("Hello"));
  REQUIRE(std::string("Hello") == to_sss("Hello"));
}

TEST_CASE("[flat_map_adapter] flat_map template alias basic test")
{
  lefticus::tools::flat_map<std::string_view, int> map;
  map["Hello World"] = 5; // NOLINT Magic Number
  ++map["Hello World"];
  REQUIRE(map["Hello World"] == 6); // NOLINT Magic Number
}


constexpr auto make_map()
{
  using namespace std::literals::string_view_literals;

  lefticus::tools::
    flat_map_adapter<std::string_view, int, lefticus::tools::simple_stack_vector<std::pair<std::string_view, int>, 10>> // NOLINT Magic Number
      map;

  map["black"sv] = 7; // NOLINT Magic Number
  map["blue"sv] = 3; // NOLINT Magic Number
  map["cyan"sv] = 5; // NOLINT Magic Number
  map["green"sv] = 2; // NOLINT Magic Number
  map["magenta"sv] = 6; // NOLINT Magic Number
  map["red"sv] = 1; // NOLINT Magic Number
  map["white"sv] = 8; // NOLINT Magic Number
  map["yellow"sv] = 4; // NOLINT Magic Number

  return map;
}

TEST_CASE("[flat_map_adapter] with simple_stack_vector backing works")
{
  // static const auto map = std::map<std::string_view, int>{color_values.begin(), color_values.end()};
  static constexpr auto map = make_map();
  // Map<std::string_view, int, color_values.size()>{{color_values}};

  REQUIRE(map.at("green") == 2);
}

TEST_CASE("[flat_map_adapter] simple_stack_flat_map alias works")
{
  using namespace std::literals::string_view_literals;

  lefticus::tools::
    flat_map_adapter<std::string_view, int, lefticus::tools::simple_stack_vector<std::pair<std::string_view, int>, 10>> // NOLINT Magic Number
      map;

  map["black"sv] = 7; // NOLINT Magic Number
  map["blue"sv] = 3; // NOLINT Magic Number
  map["cyan"sv] = 5; // NOLINT Magic Number
  map["green"sv] = 2; // NOLINT Magic Number
  map["magenta"sv] = 6; // NOLINT Magic Number
  map["red"sv] = 1; // NOLINT Magic Number
  map["white"sv] = 8; // NOLINT Magic Number
  map["yellow"sv] = 4; // NOLINT Magic Number

  REQUIRE(map.at("green") == 2);
}

TEST_CASE("[flat_map_adapter] invariants hold")// NOLINT (cognitive complexity)
{
  using namespace std::literals::string_view_literals;

  lefticus::tools::flat_map<std::string_view, int> map;

  REQUIRE(map.empty());
  REQUIRE(map.size() == 0);// NOLINT use empty

  map["black"sv] = 7; // NOLINT Magic Number

  REQUIRE(map.size() == 1);
  REQUIRE(!map.empty());
  REQUIRE(map.begin()->first == "black");
  REQUIRE(map.begin()->second == 7); // NOLINT Magic Number

  map["blue"sv] = 3; // NOLINT Magic Number
  map["cyan"sv] = 5; // NOLINT Magic Number
  map["green"sv] = 2; // NOLINT Magic Number
  map["magenta"sv] = 6; // NOLINT Magic Number
  map["red"sv] = 1; // NOLINT Magic Number
  map["white"sv] = 8; // NOLINT Magic Number
  map["yellow"sv] = 4; // NOLINT Magic Number

  REQUIRE(map.size() == 8); // NOLINT Magic Number
  REQUIRE(map["green"] == 2);
  REQUIRE(map.at("green") == 2);

  map.clear();

  REQUIRE(map.empty());
  REQUIRE(map.size() == 0);// NOLINT use empty
}

TEST_CASE("[flat_map_adapter] stackify level 1")// NOLINT (cognitive complexity)
{
  const lefticus::tools::flat_map<int, int> map{ { 1, 2 }, { 4, 5 } }; // NOLINT Magic Number
  const auto stack_map = stackify<4>(map);
  REQUIRE(stack_map.size() == 2);
  REQUIRE(stack_map.at(4) == 5); // NOLINT Magic Number
}

TEST_CASE("[flat_map_adapter] stackify level 2")// NOLINT (cognitive complexity)
{
  const lefticus::tools::flat_map<std::string, int> map{ { "Hello", 2 }, { "World", 5 } };
  const auto stack_map = stackify<16>(map); // NOLINT Magic Number
  static_assert(std::is_same_v<typename decltype(stack_map)::key_type, lefticus::tools::simple_stack_string<16>>); // NOLINT Magic Number

  REQUIRE(stack_map.size() == 2);
  REQUIRE(stack_map.at("World") == 5); // NOLINT Magic Number
}

TEST_CASE("[flat_map_adapter] stackify level 3")// NOLINT (cognitive complexity)
{
  const lefticus::tools::flat_map<std::string, std::string> map{ { "Hello", "There" }, { "World", "People" } };
  const auto stack_map = stackify<16>(map); // NOLINT Magic Number
  static_assert(std::is_same_v<typename decltype(stack_map)::key_type, lefticus::tools::simple_stack_string<16>>); // NOLINT Magic Number
  static_assert(std::is_same_v<typename decltype(stack_map)::mapped_type, lefticus::tools::simple_stack_string<16>>); // NOLINT Magic Number

  const auto max_sizes = lefticus::tools::max_element_size(stack_map);

  REQUIRE(stack_map.size() == 2);
  REQUIRE(stack_map.at("World") == "People");

  REQUIRE(max_sizes.first == 2);
  REQUIRE(max_sizes.second.first == 5); // NOLINT Magic Number
  REQUIRE(max_sizes.second.second == 6); // NOLINT Magic Number
}

TEST_CASE("[flat_map_adapter] stackify level 4")// NOLINT (cognitive complexity)
{
  const lefticus::tools::flat_map<std::string, std::vector<std::string>> map{
    { "Hello", { "1", "2", "3" } }, { "World", { "1", "22", "333", "444" } }, { "a", { "a longer string" } }
  };

  const auto stack_map = stackify<16>(map); // NOLINT Magic Number

  const auto max_sizes = lefticus::tools::max_element_size(stack_map);

  REQUIRE(max_sizes.first == 3); // NOLINT Magic Number
  REQUIRE(max_sizes.second.first == 5); // NOLINT Magic Number
  REQUIRE(max_sizes.second.second.first == 4); // NOLINT Magic Number
  REQUIRE(max_sizes.second.second.second == 15); // NOLINT Magic Number
}
