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


#ifndef LEFTICUS_TOOLS_FORWARDING_REF_HPP
#define LEFTICUS_TOOLS_FORWARDING_REF_HPP

#include <type_traits>

#include "lifetimebound.hpp"

namespace lefticus::tools {

template<typename T> struct [[nodiscard]] forwarding_ref
{
  using pointer_type = std::add_pointer_t<std::remove_reference_t<T>>;
  using reference_type = T;

  forwarding_ref(forwarding_ref &&) = delete (
    "you accidentally moved a ref (auto obj = std::move(ref)) instead of (Type obj = ref)");
  forwarding_ref(const forwarding_ref &) = delete (
    "you accidentally copied a ref (auto obj = ref) instead of (Type obj = ref)");
  forwarding_ref &operator=(forwarding_ref &&) = delete (
    "you accidentally move assigned a ref (ref = std::move(other_ref))");
  forwarding_ref &operator=(const forwarding_ref &) = delete ("you accidentally copy assigned a ref (ref = other_ref)");

  [[nodiscard]] explicit constexpr forwarding_ref(reference_type ref_ LIFETIMEBOUND) noexcept : ref{ &ref_ } {}
  [[nodiscard]] constexpr operator reference_type() noexcept LIFETIMEBOUND { return static_cast<reference_type>(*ref); }

private:
  pointer_type ref;
};

template<typename T> forwarding_ref(T &&) -> forwarding_ref<T &&>;


}// namespace lefticus::tools

#endif
