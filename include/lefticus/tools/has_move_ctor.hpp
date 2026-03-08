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


#ifndef LEFTICUS_TOOLS_HAS_MOVE_CTOR_HPP
#define LEFTICUS_TOOLS_HAS_MOVE_CTOR_HPP

#include <concepts>

namespace lefticus::tools {

template<class P> struct DetectMoveConstruction
{
  operator P const &();
  operator P &&();
};

template<typename T>
concept copyable_xor_moveable = requires(T t, DetectMoveConstruction<T> m)
{
  // borrowed from
  // https://stackoverflow.com/questions/51901837/how-to-get-if-a-type-is-truly-move-constructible/51912859#51912859
  // if this line below compiles then we know we only have either
  // a move assignment or a copy assignment, otherwise
  // it would be ambiguous
  // we cannot detect the constructor because MSVC
  // seems to have a bug, but we can detect assignment!
  t = m;
};

template<typename T>
concept has_move_ctor = std::move_constructible<T> && !copyable_xor_moveable<T>;

}// namespace lefticus::tools

#endif
