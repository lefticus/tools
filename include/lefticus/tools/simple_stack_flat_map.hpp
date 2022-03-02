#ifndef TOOLS_SIMPLE_STACK_FLAT_MAP_HPP
#define TOOLS_SIMPLE_STACK_FLAT_MAP_HPP

#include "flat_map_adapter.hpp"
#include "simple_stack_vector.hpp"

namespace lefticus::tools {
// If you use this alias, the Key is not `const` because of limitations with
// simple_stack_vector. If you dare change the Key you are taking a risk.
template<typename Key, typename Value, std::size_t Size>
using simple_stack_flat_map = flat_map_adapter<Key, Value, simple_stack_vector<std::pair<Key, Value>, Size>>;

template<std::size_t MaxSize, typename Key, typename Value, typename Container>
auto stackify(const flat_map_adapter<Key, Value, Container> &map)
{
  return simple_stack_flat_map<decltype(stackify<MaxSize>(std::declval<Key>())),
    decltype(stackify<MaxSize>(std::declval<Value>())),
    MaxSize>{ map.begin(), map.end() };
}

template<std::size_t MaxSize, typename Key, typename Value, std::size_t CurSize>
auto stackify(const simple_stack_flat_map<Key, Value, CurSize> &map)
{
  return simple_stack_flat_map<decltype(stackify<MaxSize>(std::declval<Key>())),
    decltype(stackify<MaxSize>(std::declval<Value>())),
    CurSize>{ map.begin(), map.end() };
}

}// namespace lefticus::tools

#endif// TOOLS_SIMPLE_STACK_FLAT_MAP_HPP
