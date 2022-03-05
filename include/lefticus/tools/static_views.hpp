#ifndef LEFTICUS_TOOLS_STATIC_VIEWS_HPP
#define LEFTICUS_TOOLS_STATIC_VIEWS_HPP

#include "simple_stack_flat_map.hpp"
#include "simple_stack_string.hpp"
#include "simple_stack_vector.hpp"
#include "utility.hpp"
#include <algorithm>
#include <array>
#include <span>
#include <string_view>

namespace lefticus::tools {

// max size object we can convert
static constexpr std::size_t oversized_size = 10 * 1024;


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


template<typename Value> struct oversized_array
{
  std::array<Value, oversized_size> data{};
  std::size_t size{};

  using value_type = Value;

  constexpr auto begin() const noexcept { return data.begin(); }
  constexpr auto end() const noexcept { return std::next(data.begin(), static_cast<std::ptrdiff_t>(size)); }
};

template<typename Data> constexpr auto to_oversized_array(const Data &str)
{
  oversized_array<typename Data::value_type> result;
  std::copy(str.begin(), str.end(), result.data.begin());
  result.size = str.size();
  return result;
}

consteval auto to_right_sized_array(creates_iterable auto callable)
{
  constexpr auto oversized = to_oversized_array(callable());

  using Value_Type = typename std::decay_t<decltype(oversized)>::value_type;
  std::array<Value_Type, oversized.size> result;
  std::copy(oversized.begin(), oversized.end(), result.begin());
  return result;
}

template<auto Data> constexpr const auto &make_static = Data;

consteval auto to_string_view(creates_string_like auto callable)
{
  constexpr auto &static_data = make_static<to_right_sized_array(callable)>;
  using Value_Type = typename std::decay_t<decltype(static_data)>::value_type;
  std::basic_string_view<Value_Type> result(static_data.begin(), static_data.end());
  return result;
}

consteval auto to_span(creates_iterable auto callable)
{
  constexpr auto &static_data = make_static<to_right_sized_array(callable)>;
  using Value_Type = typename std::decay_t<decltype(static_data)>::value_type;
  std::span<const Value_Type> result(static_data.begin(), static_data.end());
  return result;
}


template<std::size_t MaxSize> constexpr auto stackify(auto value) { return value; }


template<std::size_t MaxSize, typename CharType, std::size_t CurSize>
constexpr auto stackify(const basic_simple_stack_string<CharType, CurSize> &string)
{
  return string;
}

template<std::size_t MaxSize, typename CharType> constexpr auto stackify(const std::basic_string<CharType> &string)
{
  return basic_simple_stack_string<CharType, MaxSize>{ string.begin(), string.end() };
}


template<std::size_t MaxSize, typename Value> constexpr auto stackify(const std::vector<Value> &vec)
{
  return simple_stack_vector<decltype(stackify<MaxSize>(std::declval<Value>())), MaxSize>{ vec.begin(), vec.end() };
}

template<std::size_t MaxSize, typename Value, std::size_t CurSize>
constexpr auto stackify(const simple_stack_vector<Value, CurSize> &vec)
{
  return simple_stack_vector<decltype(stackify<MaxSize>(std::declval<Value>())), CurSize>{ vec.begin(), vec.end() };
}


template<std::size_t MaxSize, typename Key, typename Value, typename Container>
constexpr auto stackify(const flat_map_adapter<Key, Value, Container> &map)
{
  return simple_stack_flat_map<decltype(stackify<MaxSize>(std::declval<Key>())),
    decltype(stackify<MaxSize>(std::declval<Value>())),
    MaxSize>{ map.begin(), map.end() };
}

template<std::size_t MaxSize, typename Key, typename Value, std::size_t CurSize>
constexpr auto stackify(const simple_stack_flat_map<Key, Value, CurSize> &map)
{
  return simple_stack_flat_map<decltype(stackify<MaxSize>(std::declval<Key>())),
    decltype(stackify<MaxSize>(std::declval<Value>())),
    CurSize>{ map.begin(), map.end() };
}


template<typename Arg> constexpr auto max_max(const Arg &lhs, const Arg &rhs) { return std::max(lhs, rhs); }

template<typename First, typename Second>
constexpr auto max_max(const pair<First, Second> &lhs, const pair<First, Second> &rhs)
{
  return pair{ max_max(lhs.first, rhs.first), max_max(lhs.second, rhs.second) };
}

constexpr auto max_element_size([[maybe_unused]] auto value) { return 1; }

template<typename CharType, std::size_t CurSize>
constexpr auto max_element_size(const basic_simple_stack_string<CharType, CurSize> &string)
{
  return string.size();
}

template<typename Value, std::size_t CurSize>
constexpr auto max_element_size(const simple_stack_vector<Value, CurSize> &vec)
{
  decltype(max_element_size(std::declval<Value>())) child_max{};

  for (const auto &elem : vec) { child_max = max_max(child_max, max_element_size(elem)); }
  return pair{ vec.size(), child_max };
}

template<typename Key, typename Value, std::size_t CurSize>
constexpr auto max_element_size(const simple_stack_flat_map<Key, Value, CurSize> &map)
{
  decltype(max_element_size(std::declval<Key>())) key_max{};
  decltype(max_element_size(std::declval<Value>())) value_max{};

  for (const auto &elem : map) {
    key_max = max_max(key_max, max_element_size(elem.first));
    value_max = max_max(value_max, max_element_size(elem.second));
  }

  return pair{ map.size(), pair{ key_max, value_max } };
}

template<auto NewSize> constexpr auto resize(const auto &value) { return value; }

template<auto NewSize, typename CharType, std::size_t CurSize>
constexpr auto resize(const basic_simple_stack_string<CharType, CurSize> &str)
{
  // +1 for null terminator
  return basic_simple_stack_string<CharType, NewSize + 1>{ str.begin(), str.end() };
}

template<auto NewSize, typename Value, std::size_t CurSize>
constexpr auto resize(const simple_stack_vector<Value, CurSize> &vec)
{
  using new_value_type = decltype(resize<NewSize.second>(std::declval<Value>()));
  return simple_stack_vector<new_value_type, NewSize.first>{ vec.begin(), vec.end() };
}

template<auto NewSize, typename Key, typename Value, std::size_t CurSize>
constexpr auto resize(const simple_stack_flat_map<Key, Value, CurSize> &map)
{
  using new_key_type = decltype(resize<NewSize.second.first>(std::declval<Key>()));
  using new_value_type = decltype(resize<NewSize.second.second>(std::declval<Value>()));

  return simple_stack_flat_map<new_key_type, new_value_type, NewSize.first>{ map.begin(), map.end() };
}

template<std::size_t MaxSize, typename Callable> constexpr auto minimized_stackify(Callable callable)
{
  constexpr auto stackified{ stackify<MaxSize>(callable()) };
  constexpr auto sizes{ max_element_size(stackified) };
  return resize<sizes>(stackified);
}


}// namespace lefticus::tools

#endif
