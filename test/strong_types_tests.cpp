//
// Created by jason on 3/12/24.
//


#include <lefticus/tools/strong_types.hpp>

// we could have auto detected the capabilities, by using requires
// but that would have enabled more operations than we wanted.

struct Point
{
  float x;
  float y;
  [[nodiscard]] constexpr auto operator<=>(const Point &) const noexcept = default;
};

[[nodiscard]] constexpr Point operator*(Point lhs, float scalar) noexcept
{
  return Point{ lhs.x * scalar, lhs.y * scalar };
}

[[nodiscard]] constexpr Point operator*(float scalar, Point p) noexcept { return Point{ p.x * scalar, p.y * scalar }; }

[[nodiscard]] constexpr Point operator+(Point lhs, Point rhs) noexcept { return Point{ lhs.x + rhs.x, lhs.y + rhs.y }; }

[[nodiscard]] constexpr Point operator-(Point lhs, Point rhs) noexcept { return Point{ lhs.x - rhs.x, lhs.y - rhs.y }; }


/*
using Player_Position = strong_alias<int, struct PP>;
using Offset = strong_alias<int, struct Off>;

auto plus(Player_Position, Offset) -> Player_Position;
auto plus(Offset, Player_Position) -> Player_Position;
auto minus(Player_Position, Offset) -> Player_Position;
auto minus(Offset, Player_Position) -> Player_Position;
auto equate(Player_Position, Player_Position) -> bool;
auto order(Player_Position, Player_Position) -> bool;
*/

using Position = strong_alias<Point, struct Pos>;
using Time = strong_alias<float, struct Tim>;

auto order(Position, Position) -> bool;
auto order(Time, Time) -> bool;

using Player1_Position = strong_alias<Point, struct Pos1>;
using Player1_Velocity = strong_alias<Point, struct Vel1>;
using Player1_Displacement = strong_alias<Point, struct Dis1>;

auto add(Player1_Position, Player1_Displacement) -> Player1_Position;
auto add(Player1_Displacement, Player1_Position) -> Player1_Position;
auto multiply(Time, Player1_Velocity) -> Player1_Displacement;
auto multiply(Player1_Velocity, Time) -> Player1_Displacement;

using Player2_Position = strong_alias<Point, struct Pos2>;
using Player2_Velocity = strong_alias<Point, struct Vel2>;
using Player2_Displacement = strong_alias<Point, struct Dis2>;

auto add(Player2_Position, Player2_Displacement) -> Player2_Position;
auto add(Player2_Displacement, Player2_Position) -> Player2_Position;
auto multiply(Time, Player2_Velocity) -> Player2_Displacement;
auto multiply(Player2_Velocity, Time) -> Player2_Displacement;

auto casts(Player1_Position) -> Position;
auto casts(Player2_Position) -> Position;

auto update_position(auto position, auto velocity, Time t) { return position + velocity * t; }

struct Avatar
{
};
struct Player1
{
};
struct Player2
{
};

void draw(Avatar, Position);

auto get_avatar(Player1, Time) -> Avatar;
auto get_avatar(Player2, Time) -> Avatar;

auto get_position(Player1) -> Player1_Position;
auto get_position(Player2) -> Player2_Position;

auto get_displacement(Player1) -> Player1_Displacement;
auto get_displacement(Player2) -> Player2_Displacement;

auto get_velocity(Player1) -> Player1_Velocity;
auto get_velocity(Player2) -> Player2_Velocity;

auto get_player1() -> Player1;
auto get_player2() -> Player2;

void draw_players(Time t)
{
  auto do_draw = [t](const auto &player) {
    draw(get_avatar(player, t), strong_cast<Position>(update_position(get_position(player), get_velocity(player), t)));
  };

  do_draw(get_player1());
  do_draw(get_player1());
}

using Pattern = strong_alias<std::string, struct Pat>;
using Directory = strong_alias<std::filesystem::path, struct Dir, []([[maybe_unused]] const std::filesystem::path &p) {
  [[maybe_unused]] std::error_code ec;
  assert(!std::filesystem::is_regular_file(p, ec));
}>;

using File = strong_alias<std::filesystem::path, struct Fil, []([[maybe_unused]] const std::filesystem::path &p) {
  [[maybe_unused]] std::error_code ec;
  assert(!std::filesystem::is_directory(p, ec));
}>;

auto order(File, File) -> bool;
auto equate(File, File) -> bool;
auto order(Directory, Directory) -> bool;
auto equate(Directory, Directory) -> bool;
auto order(Pattern, Pattern) -> bool;
auto equate(Pattern, Pattern) -> bool;
auto divide(Directory, Directory) -> Directory;
auto divide(Directory, File) -> File;

bool matches(const Pattern &pattern, const std::filesystem::path &path);

std::optional<File> find_first_file(const Directory &dir, const Pattern &pattern)
{
  for (const auto &path : std::filesystem::directory_iterator{ dir.get() } | std::views::filter([](const auto &f) {
         return std::filesystem::is_regular_file(f);
       }) | std::views::filter(std::bind_front(matches, pattern))) {
    return File{ path };
  }

  return {};
}

template<typename LHS, typename RHS> consteval bool types_equatable()
{
  return requires(LHS lhs, RHS rhs) { lhs == rhs; };
}

int main()
{
  constexpr Player1_Position p{ 2.0f, 3.0f };
  constexpr Player1_Velocity v{ -1.0f, 2.0f };
  constexpr Time t{ 4.0f };

  static_assert(std::is_same_v<decltype(v * t), Player1_Displacement>);
  static_assert(std::is_same_v<decltype(t * v), Player1_Displacement>);
  static_assert(std::is_same_v<decltype(p + (t * v)), Player1_Position>);

  static_assert(!types_equatable<Player1_Position, Player1_Position>());
  static_assert(!types_equatable<Player1_Velocity, Player1_Position>());
  static_assert(!types_equatable<Player1_Velocity, Player1_Velocity>());

  static_assert((p + (t * v)).get() == Point{ 2.0f + (-1.0f * 4.0f), 3.0f + (2.0f * 4.0f) });

  auto f = find_first_file(Directory{ "/home/jason" }, Pattern{ "*.txt" });

  draw_players(Time{ 4.3f });
}

