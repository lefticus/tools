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


#ifndef LEFTICUS_TOOLS_UTILITY_HPP
#define LEFTICUS_TOOLS_UTILITY_HPP

#include <utility>

namespace lefticus::tools {
template<typename First, typename Second> struct pair
{
  First first;
  Second second;
};

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const pair<First, Second> &lhs, const pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const ::std::pair<First, Second> &lhs, const pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const pair<First, Second> &lhs, const ::std::pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second> pair(First f, Second s) -> pair<First, Second>;
}// namespace lefticus::tools

#endif// LEFTICUS_TOOLS_UTILITY_HPP
