#include <catch2/catch.hpp>


#include <array>
#include <utility>

#include <lefticus/tools/curry.hpp>
#include <lefticus/tools/lambda_coroutines.hpp>
#include <lefticus/tools/simple_stack_string.hpp>
#include <lefticus/tools/simple_stack_vector.hpp>
#include <lefticus/tools/flat_map.hpp>

#include <catch2/catch.hpp>


TEST_CASE("Generate infinite sequence", "[lambda_coroutines]")// NOLINT (cognitive complexity)
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

TEST_CASE("Cooperative multi tasking")// NOLINT (cognitive complexity)
{
  enum OpCodes : std::uint8_t { ADD = 0, STA = 1, NOP = 2 };
  struct Machine
  {
    std::uint8_t PC{ 0 };
    std::uint8_t A{ 0 };
    std::array<uint8_t, 256> RAM{ STA, 10, ADD, 15 };
  };

  Machine machine;

  auto CPU = [state = 0, &machine, op = OpCodes::NOP]() mutable {
    lambda_co_begin(state);

    while (true) {
      op = static_cast<OpCodes>(machine.RAM.at(machine.PC));
      ++machine.PC;
      if (op == OpCodes::STA) {
        lambda_co_yield();
        machine.A = machine.RAM.at(machine.PC++);
        lambda_co_yield();
      } else if (op == OpCodes::ADD) {
        lambda_co_yield();
        machine.A += machine.RAM.at(machine.PC++);
        lambda_co_yield();
      } else if (op == OpCodes::NOP) {
        lambda_co_yield();
      };
    }

    lambda_co_end();
  };

  CPU();
  REQUIRE(machine.PC == 1);
  REQUIRE(machine.A == 0);
  CPU();
  REQUIRE(machine.PC == 2);
  REQUIRE(machine.A == 10);
  CPU();
  REQUIRE(machine.PC == 3);
  REQUIRE(machine.A == 10);
  CPU();
  REQUIRE(machine.PC == 4);
  REQUIRE(machine.A == 25);
}


struct Data
{
  std::string value;
};

TEST_CASE("curry member object pointer w/return reference forwarding")
{
  Data d;
  auto member = lefticus::tools::curry(&Data::value);

  member(&d) = "Hello World";

  REQUIRE(d.value == "Hello World");

  std::string &value = member(&d);

  value = "A long string to test";

  REQUIRE(member(&d) == "A long string to test");
}


TEST_CASE("simple_stack_string simple_stack_string == std::string, std::string == sss")
{
  using namespace lefticus::tools::literals;
  REQUIRE(to_sss("Hello") == std::string("Hello"));
  REQUIRE(std::string("Hello") == to_sss("Hello"));
}

TEST_CASE("flat_map basic test")
{
  lefticus::tools::flat_map < std::string_view, int> m;
  m["Hello World"] = 5;
  ++m["Hello World"];
  REQUIRE(m["Hello World"] == 6);
}


constexpr auto make_map()
{
  using namespace std::literals::string_view_literals;

  lefticus::tools::flat_map<std::string_view, int, lefticus::tools::simple_stack_vector<std::pair<std::string_view, int>, 10>> m;

  m["black"sv] = 7;
  m["blue"sv] = 3;
  m["cyan"sv] = 5;
  m["green"sv] = 2;
  m["magenta"sv] = 6;
  m["red"sv] = 1;
  m["white"sv] = 8;
  m["yellow"sv] = 4;

  return m;
}

TEST_CASE("flat_map with simple_stack_vector backing works")
{
    // static const auto map = std::map<std::string_view, int>{color_values.begin(), color_values.end()};
    static constexpr auto map = make_map();
    // Map<std::string_view, int, color_values.size()>{{color_values}};

    [[maybe_unused]] const auto result = map.at("green");
}