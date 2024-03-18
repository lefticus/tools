/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/


//
// Created by jason on 3/12/24.
//

#ifndef LEFTICUS_TOOLS_STRONG_TYPES_HPP
#define LEFTICUS_TOOLS_STRONG_TYPES_HPP

#include <cassert>
#include <compare>
#include <filesystem>
#include <functional>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace lefticus::tools {
template<typename LHS, typename RHS>
concept addable = requires(LHS lhs, RHS rhs) { add(lhs, rhs); };

template<typename LHS, typename RHS>
concept subtractable = requires(LHS lhs, RHS rhs) { subtract(lhs, rhs); };

template<typename LHS, typename RHS>
concept multipliable = requires(LHS lhs, RHS rhs) { multiply(lhs, rhs); };

template<typename LHS, typename RHS>
concept dividable = requires(LHS lhs, RHS rhs) { divide(lhs, rhs); };

template<typename LHS>
concept negatable = requires(LHS lhs) { negate(lhs); };

template<typename LHS, typename RHS>
concept orderable = requires(LHS lhs, RHS rhs) { order(lhs, rhs); };

template<typename LHS, typename RHS>
concept equatable = requires(LHS lhs, RHS rhs) { equate(lhs, rhs); };

template<typename From, typename To>
concept casts_to = requires(const From &from) {
  {
    casts(from)
  } -> std::same_as<To>;
};


template<typename Underlying> constexpr void null_validator(const Underlying &) {}

template<typename Underlying, typename Tag, auto Validator = &null_validator<Underlying>> struct strong_alias
{
  template<typename... Param>
  constexpr explicit strong_alias(Param &&...param)
    requires std::is_constructible_v<Underlying, decltype(param)...>
    : data(std::forward<Param>(param)...)
  {
    Validator(data);
  }

  [[nodiscard]] constexpr const Underlying &get() const & noexcept { return data; }
  [[nodiscard]] constexpr Underlying &&get() && noexcept { return std::move(data); }
  [[nodiscard]] constexpr Underlying &get() & noexcept { return data; }

  [[nodiscard]] auto operator<=>(const strong_alias &rhs) const noexcept
    requires(orderable<strong_alias, strong_alias>)
  = default;

  [[nodiscard]] bool operator==(const strong_alias &rhs) const noexcept
    requires(equatable<strong_alias, strong_alias>)
  = default;

  template<typename U2, typename T2>
  [[nodiscard]] constexpr auto operator<=>(const strong_alias<U2, T2> &rhs) const noexcept
    requires(orderable<strong_alias, strong_alias<U2, T2>>)
  {
    return data <=> rhs.get();
  }
  template<typename U2, typename T2>
  [[nodiscard]] constexpr bool operator==(const strong_alias<U2, T2> &rhs) const noexcept
    requires(equatable<strong_alias, strong_alias<U2, T2>>)
  {
    return data == rhs.get();
  }

private:
  Underlying data;
};

template<typename LHS, typename RHS>
[[nodiscard]] constexpr auto operator-(LHS &&lhs) noexcept(noexcept(-lhs.get())) -> decltype(negate(lhs))
  requires(negatable<LHS>)
{
  return decltype(negate(lhs)){ std::forward<LHS>(lhs).get() };
}

template<typename LHS, typename RHS>
[[nodiscard]] constexpr auto operator+(LHS &&lhs, RHS &&rhs) noexcept(noexcept(lhs.get() + rhs.get()))
  -> decltype(add(lhs, rhs))
  requires(addable<LHS, RHS>)
{
  return decltype(add(lhs, rhs)){ std::forward<LHS>(lhs).get() + std::forward<RHS>(rhs).get() };
}

template<typename LHS, typename RHS>
[[nodiscard]] constexpr auto operator-(LHS &&lhs, RHS &&rhs) noexcept(noexcept(lhs.get() - rhs.get()))
  -> decltype(subtract(lhs, rhs))
  requires(subtractable<LHS, RHS>)
{
  return decltype(subtract(lhs, rhs)){ std::forward<LHS>(lhs).get() - std::forward<RHS>(rhs).get() };
}

template<typename LHS, typename RHS>
[[nodiscard]] constexpr auto operator*(LHS &&lhs, RHS &&rhs) noexcept(noexcept(lhs.get() * rhs.get()))
  -> decltype(multiply(lhs, rhs))
  requires(multipliable<LHS, RHS>)
{
  return decltype(multiply(lhs, rhs)){ std::forward<LHS>(lhs).get() * std::forward<RHS>(rhs).get() };
}

template<typename LHS, typename RHS>
[[nodiscard]] constexpr auto operator/(LHS &&lhs, RHS &&rhs) noexcept(noexcept(lhs.get() / rhs.get()))
  -> decltype(divide(lhs, rhs))
  requires(dividable<LHS, RHS>)
{
  return decltype(divide(lhs, rhs)){ std::forward<LHS>(lhs).get() / std::forward<RHS>(rhs).get() };
}

template<typename To, typename From>
[[nodiscard]] constexpr auto strong_cast(From &&from) -> To
  requires(casts_to<From, To>)
{
  return To{ std::forward<From>(from).get() };
}

}// namespace lefticus::tools

#endif