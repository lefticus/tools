#ifndef LEFTICUS_TOOLS_STATIC_VIEWS_HPP
#define LEFTICUS_TOOLS_STATIC_VIEWS_HPP

#include <algorithm>
#include <array>
#include <span>
#include <string_view>

namespace lefticus::tools {


template<typename Value>
concept is_iterable = requires(const Value &value)
{
  value.begin();
  value.end();
};

template<typename Callable>
concept creates_iterable = requires(const Callable &callable)
{
  requires is_iterable<std::decay_t<decltype(callable())>>;
};


template<typename Callable>
concept creates_string_like = requires(const Callable &callable)
{
  requires creates_iterable<Callable>;
  // TODO this check needs to be better
  typename std::decay_t<decltype(callable())>::traits_type;
};

consteval auto to_right_sized_array(creates_iterable auto callable)
{
  const auto tmp = callable();
  using Value_Type = typename std::decay_t<decltype(tmp)>::value_type;
  std::array<Value_Type, callable().size()> result;
  std::copy(tmp.begin(), tmp.end(), result.begin());
  return result;
}

template<auto Data> consteval const auto &make_static()
{
  // this is fully allowed - Data is made into a static by the compiler
  // cppcheck-suppress returnTempReference
  return Data;
}

consteval auto to_string_view(creates_string_like auto callable)
{
  constexpr auto &static_data = make_static<to_right_sized_array(callable)>();
  using Value_Type = typename std::decay_t<decltype(static_data)>::value_type;
  std::basic_string_view<Value_Type> result(static_data.begin(), static_data.end());
  return result;
}

consteval auto to_span(creates_iterable auto callable)
{
  constexpr auto &static_data = make_static<to_right_sized_array(callable)>();
  using Value_Type = typename std::decay_t<decltype(static_data)>::value_type;
  std::span<const Value_Type> result(static_data.begin(), static_data.end());
  return result;
}

}// namespace lefticus::tools

#endif
