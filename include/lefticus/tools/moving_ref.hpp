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


#ifndef LEFTICUS_TOOLS_MOVING_REF_HPP
#define LEFTICUS_TOOLS_MOVING_REF_HPP

#include "has_move_ctor.hpp"
#include "lifetimebound.hpp"

namespace lefticus::tools {

template<typename T>
concept movable = std::is_move_constructible_v<T> && std::is_move_assignable_v<T> && not std::is_const_v<
                    T> && not std::is_pointer_v<T> && not std::is_reference_v<T>;

template<typename T>
requires movable<T> && has_move_ctor<T>
struct [[nodiscard]] moving_ref
{
  using value_type = T;
  using pointer_type = std::add_pointer_t<T>;
  using reference_type = std::add_rvalue_reference_t<T>;

  moving_ref(moving_ref &&) = delete (
    "you accidentally moved a ref (auto obj = std::move(ref)) instead of (Type obj = ref)");
  moving_ref(const moving_ref &) = delete (
    "you accidentally copied a ref (auto obj = ref) instead of (Type obj = ref)");
  moving_ref &operator=(moving_ref &&) = delete ("you accidentally move assigned a ref (ref = std::move(other_ref))");
  moving_ref &operator=(const moving_ref &) = delete ("you accidentally copy assigned a ref (ref = other_ref)");

  [[nodiscard]] explicit constexpr moving_ref(reference_type ref_ LIFETIMEBOUND) noexcept : ref{ &ref_ } {}

  moving_ref(const T &) = delete ("this type is only to be used with rvalue reference parameters");

  [[nodiscard]] constexpr operator reference_type() noexcept LIFETIMEBOUND { return static_cast<reference_type>(*ref); }
  [[nodiscard]] constexpr reference_type take() noexcept LIFETIMEBOUND { return static_cast<reference_type>(*ref); }


private:
  pointer_type ref;
};

}// namespace lefticus::tools

#endif
