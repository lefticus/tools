#ifndef TOOLS_FLAT_MAP_HPP
#define TOOLS_FLAT_MAP_HPP

#include "flat_map_adapter.hpp"
#include "utility.hpp"
#include <vector>

namespace lefticus::tools {
template<typename Key, typename Value>
using flat_map = flat_map_adapter<Key, Value, std::vector<pair<const Key, Value>>>;
}

#endif// TOOLS_FLAT_MAP_HPP
