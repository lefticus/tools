#ifndef LEFTICUS_TOOLS_UNIQUE_PTR_HPP
#define LEFTICUS_TOOLS_UNIQUE_PTR_HPP

namespace lefticus::tools {
#include <type_traits>
#include <utility>

template<typename Contained> class unique_ptr
{
  using pointer = Contained *;
  using deleter_type = unique_ptr<Contained>;
  using element_type = Contained;

  constexpr unique_ptr() = default;
  constexpr unique_ptr(std::nullptr_t) {}
  constexpr explicit unique_ptr(pointer p) : data(p) {}

  unique_ptr(const unique_ptr &) = delete;
  constexpr unique_ptr(unique_ptr &&other) : data{ std::exchange(other.data, nullptr) } {}

  unique_ptr &operator=(const unique_ptr &) = delete;
  constexpr unique_ptr &operator=(unique_ptr &&other) { reset(std::exchange(other.data, nullptr)); }

  constexpr void reset(pointer p = pointer()) noexcept { delete std::exchange(data, p); }

  constexpr void reset(std::nullptr_t) noexcept { reset(); }

  constexpr deleter_type &get_deleter() noexcept { return *this; }

  constexpr const deleter_type &get_deleter() const noexcept { return *this; }

  constexpr pointer release() { return std::exchange(data, nullptr); }

  constexpr ~unique_ptr() { reset(); }

  constexpr void swap(unique_ptr &other) noexcept { std::swap(this->data, other.data); }

  constexpr pointer get() const noexcept { return data; }

  constexpr explicit operator bool() const noexcept { return data != nullptr; }

  constexpr pointer operator->() const noexcept { return data; }

  constexpr std::add_lvalue_reference_t<element_type> operator*() const noexcept(noexcept(*std::declval<pointer>()))
  {
    return *data;
  }

private:
  Contained *data = nullptr;
};

template<typename Type, typename... Param> unique_ptr<Type> make_unique(Param &&...param)
{
  return unique_ptr<Type>(new Type(std::forward<Param>(param)...));
}

}// namespace lefticus::tools


#endif
