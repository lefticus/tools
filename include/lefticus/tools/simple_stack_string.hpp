#ifndef LEFTICUS_TOOLS_SIMPLE_STACK_STRING_HPP
#define LEFTICUS_TOOLS_SIMPLE_STACK_STRING_HPP

#include <cstdint>
#include <string>
#include <array>

namespace lefticus::tools {
template<typename CharType, std::size_t TotalCapacity, typename Traits = std::char_traits<CharType>>
struct basic_simple_stack_string
{
  using traits_type = Traits;
  using value_type = CharType;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using data_type = std::array<value_type, TotalCapacity>;

  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reverse_iterator = typename data_type::reverse_iterator;
  using const_reverse_iterator = typename data_type::const_reverse_iterator;

  static constexpr auto total_capacity = TotalCapacity;

  constexpr basic_simple_stack_string() = default;
  constexpr basic_simple_stack_string(std::nullptr_t) = delete;

  constexpr explicit basic_simple_stack_string(std::initializer_list<value_type> data)
  {
    for (const auto &c : data) {
      push_back(c);// push_back is very cheap and this is simple
    }
  }

  template<size_t Size>
  constexpr explicit basic_simple_stack_string(const value_type (&str)[Size])
    : basic_simple_stack_string(std::basic_string_view<value_type>(str))
  {
    static_assert(Size <= TotalCapacity);
  }

  constexpr explicit basic_simple_stack_string(const std::basic_string_view<value_type> sv)
  {
    for (const auto c : sv) { push_back(c); }
  }

  constexpr operator std::basic_string_view<value_type>() const noexcept
  {
    return std::basic_string_view<value_type>(data(), size());
  }

  [[nodiscard]] constexpr value_type *data() { return data_.data(); }
  [[nodiscard]] constexpr const value_type *data() const { return data_.data(); }
  [[nodiscard]] constexpr const value_type *c_str() const { return data(); }

  [[nodiscard]] constexpr iterator begin() noexcept { return data_.begin(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data_.cbegin(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data_.cbegin(); }

  [[nodiscard]] constexpr iterator end() noexcept
  {
    return std::next(data_.begin(), static_cast<difference_type>(size_));
  }

  [[nodiscard]] constexpr const_iterator end() const noexcept
  {
    return std::next(data_.cbegin(), static_cast<difference_type>(size_));
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
  {
    return std::next(data_.rbegin(), static_cast<difference_type>(TotalCapacity - size_));
  }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
  {
    return std::next(data_.crbegin(), static_cast<difference_type>(TotalCapacity - size_));
  }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept { return data_.rend(); }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return data_.crend(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return data_.crend(); }

  constexpr value_type &push_back(const value_type c)
  {
    if (size_ - 1 == TotalCapacity) { throw std::length_error("push_back would exceed static capacity"); }
    data_[size_ + 1] = 0;// null terminator
    data_[size_] = c;
    return data_[size_++];
  }

  [[nodiscard]] constexpr value_type &operator[](const std::size_t idx) noexcept { return data_[idx]; }

  [[nodiscard]] constexpr const value_type &operator[](const std::size_t idx) const noexcept { return data_[idx]; }

  [[nodiscard]] constexpr value_type &at(const std::size_t idx)
  {
    if (idx > size_) { throw std::out_of_range("index past end of stack_vector"); }
    return data_[idx];
  }

  [[nodiscard]] constexpr const value_type &at(const std::size_t idx) const
  {
    if (idx > size_) { throw std::out_of_range("index past end of stack_vector"); }
    return data_[idx];
  }

  constexpr basic_simple_stack_string &operator+=(const std::string_view sv)
  {
    for (const auto c : sv) { push_back(c); }

    return *this;
  }

  // resets the size to 0, but does not destroy any existing objects
  constexpr void clear() { size_ = 0; }


  // cppcheck-suppress functionStatic
  constexpr void reserve(size_type new_capacity)
  {
    if (new_capacity + 1 > TotalCapacity) {
      throw std::length_error("new capacity would exceed max_size for stack_vector");
    }
  }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type capacity() noexcept { return TotalCapacity - 1; }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type max_size() noexcept { return TotalCapacity - 1; }

  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

  constexpr void resize(const size_type new_size)
  {
    if (new_size <= size_) {
      size_ = new_size;
    } else {
      if (new_size > TotalCapacity) {
        throw std::length_error("resize would exceed static capacity");
      } else {
        auto old_end = end();
        size_ = new_size;
        auto new_end = end();
        while (old_end != new_end) {
          *old_end = value_type{};
          ++old_end;
        }
      }
    }

    data_[size_] = 0;
  }

  constexpr void pop_back() noexcept
  {
    --size_;
    data_[size_] = 0;
  }

  // cppcheck-suppress functionStatic
  constexpr void shrink_to_fit() noexcept
  {
    // nothing to do here
  }


private:
  // default initializing to make it more C++17 friendly
  data_type data_{};
  size_type size_{};
};

template<typename CharType, std::size_t Size>
basic_simple_stack_string(const CharType (&)[Size]) -> basic_simple_stack_string<CharType, Size>;

template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const basic_simple_stack_string<CharType, Size> &lhs,
  const CharType *rhs) noexcept
{
  return static_cast<std::basic_string_view<CharType>>(lhs) == std::basic_string_view<CharType>(rhs);
}

template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const CharType *lhs,
  const basic_simple_stack_string<CharType, Size> &rhs) noexcept
{
  return std::basic_string_view<CharType>(lhs) == static_cast<std::basic_string_view<CharType>>(rhs);
}

template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const basic_simple_stack_string<CharType, Size> &lhs,
  const std::basic_string_view<CharType> rhs) noexcept
{
  return static_cast<std::basic_string_view<CharType>>(lhs) == rhs;
}

template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const std::basic_string_view<CharType> lhs,
  const basic_simple_stack_string<CharType, Size> &rhs) noexcept
{
  return lhs == static_cast<std::basic_string_view<CharType>>(rhs);
}


template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const basic_simple_stack_string<CharType, Size> &lhs,
  const std::basic_string<CharType> &rhs) noexcept
{
  return static_cast<std::basic_string_view<CharType>>(lhs) == rhs;
}

template<typename CharType, std::size_t Size>
[[nodiscard]] constexpr bool operator==(const std::basic_string<CharType> &lhs,
  const basic_simple_stack_string<CharType, Size> &rhs) noexcept
{
  return lhs == static_cast<std::basic_string_view<CharType>>(rhs);
}


template<typename CharType, std::size_t LHSSize, std::size_t RHSSize>
[[nodiscard]] constexpr basic_simple_stack_string<CharType, LHSSize + RHSSize - 1>
  operator+(const basic_simple_stack_string<CharType, LHSSize> &lhs, const CharType (&rhs)[RHSSize])
{
  basic_simple_stack_string<CharType, LHSSize + RHSSize - 1> result{ lhs };
  result += rhs;
  return result;
}

template<typename CharType, std::size_t LHSSize, std::size_t RHSSize>
[[nodiscard]] constexpr basic_simple_stack_string<CharType, LHSSize + RHSSize - 1>
  operator+(const CharType (&lhs)[LHSSize], const basic_simple_stack_string<CharType, RHSSize> &rhs)
{
  basic_simple_stack_string<CharType, LHSSize + RHSSize - 1> result{ lhs };
  result += rhs;
  return result;
}

template<typename CharType, std::size_t LHSSize, std::size_t RHSSize>
[[nodiscard]] constexpr basic_simple_stack_string<CharType, LHSSize + RHSSize - 1> operator+(
  const basic_simple_stack_string<CharType, LHSSize> &lhs,
  const basic_simple_stack_string<CharType, RHSSize> &rhs)
{
  basic_simple_stack_string<CharType, LHSSize + RHSSize - 1> result{ lhs };
  result += rhs;
  return result;
}


template<std::size_t TotalCapacity> using simple_stack_string = basic_simple_stack_string<char, TotalCapacity>;

namespace literals {
  // not actually a literal, but best we can do with C++17 support
  template<typename CharType, std::size_t Size> [[nodiscard]] constexpr auto to_sss(const CharType (&data)[Size])
  {
    return basic_simple_stack_string{ data };
  }
}// namespace literals

}// namespace lefticus::tools

#endif
