#ifndef LEFTICUS_TOOLS_STATIC_VIEWS_HPP
#define LEFTICUS_TOOLS_STATIC_VIEWS_HPP

#include <span>
#include <string_view>

namespace lefticus::tools {

// max size object we can convert
static constexpr std::size_t oversized_size = 10 * 1024;

template<typename Value> struct oversized_array
{
  using value_type = Value;
  std::array<Value, oversized_size> data{};
  std::size_t size{};
};

template<typename Value>
concept is_iterable = requires(const Value &value)
{
  value.begin();
  value.end();
};

constexpr auto to_oversized_array(const is_iterable auto &str)
{
  using value_type = std::decay_t<decltype(*str.begin())>;
  oversized_array<value_type> result{ {}, str.size() };
  std::copy(str.begin(), str.end(), result.data.begin());
  return result;
}

consteval auto to_array(auto callable)
{
  constexpr auto oversized = to_oversized_array(callable());
  std::array<typename std::decay_t<decltype(oversized)>::value_type, oversized.size> result;
  std::copy(oversized.data.begin(), std::next(oversized.data.begin(), oversized.size), result.begin());
  return result;
}

template<auto Data> consteval const auto &make_static() { return Data; }

template<typename Callable>
concept creates_string_like = requires(const Callable &callable)
{
  typename std::decay_t<decltype(callable())>::traits_type::char_traits;
};

template<typename Callable>
concept creates_iterable = requires(const Callable &callable)
{
  requires is_iterable<std::decay_t<decltype(callable())>>;
};

// takes a callable that returns a string-like thing and returns a basic_string_view<CharType> to static data
consteval auto to_string_view(creates_string_like auto callable)
{
  constexpr auto &static_data = make_static<to_array(callable)>();
  using char_type = typename std::decay_t<decltype(static_data)>::value_type;
  return std::basic_string_view<char_type>{ static_data.begin(), static_data.size() };
}

// takes a callable that returns an array-like thing and returns a array<const ValueType> to static data
consteval auto to_span(creates_iterable auto callable)
{
  constexpr auto &static_data = make_static<to_array(callable)>();
  using value_type = typename std::decay_t<decltype(static_data)>::value_type;
  return std::span<const value_type>{ static_data.begin(), static_data.size() };
}


}// namespace lefticus::tools

#endif
