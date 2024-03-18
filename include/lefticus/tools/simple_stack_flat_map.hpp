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


#ifndef TOOLS_SIMPLE_STACK_FLAT_MAP_HPP
#define TOOLS_SIMPLE_STACK_FLAT_MAP_HPP

#include "flat_map_adapter.hpp"
#include "simple_stack_vector.hpp"
#include "utility.hpp"

namespace lefticus::tools {
// If you use this alias, the Key is not `const` because of limitations with
// simple_stack_vector. If you dare change the Key you are taking a risk.
template<typename Key, typename Value, std::size_t Size>
using simple_stack_flat_map = flat_map_adapter<Key, Value, simple_stack_vector<pair<Key, Value>, Size>>;


}// namespace lefticus::tools

#endif// TOOLS_SIMPLE_STACK_FLAT_MAP_HPP
