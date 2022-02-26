#ifndef LEFTICUS_TOOLS_SIMPLE_STACK_VECTOR_HPP
#define LEFTICUS_TOOLS_SIMPLE_STACK_VECTOR_HPP


namespace lefticus::tools {

#include <array>
#include <cstdint>
#include <stdexcept>

template<typename T>
concept default_constructible = std::is_default_constructible_v<T>;

// changes from std::vector
//  * capacity if fixed at compile-time
//  * it never allocates
//  * items must be default constructible
//  * items are never destroyed until the entire stack_vector
//    is destroyed.
//  * iterators are never invalidated
//  * capacity() and max_size() are now static functions
//  * should be fully C++17 usable within constexpr
// TODO add C++17 binary to test this simple_stack_vector
template<default_constructible Contained, std::size_t Capacity> struct simple_stack_vector
{
  using value_type = Contained;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using data_type = std::array<value_type, Capacity>;

  using iterator = typename data_type::iterator;
  using const_iterator = typename data_type::const_iterator;
  using reverse_iterator = typename data_type::reverse_iterator;
  using const_reverse_iterator = typename data_type::const_reverse_iterator;

  constexpr simple_stack_vector() = default;
  constexpr explicit simple_stack_vector(std::initializer_list<value_type> values) : size_{ values.size() }
  {
    size_type index = 0;
    for (const auto &value : values) { data_[index++] = value; }
  }

  [[nodiscard]] constexpr iterator begin() noexcept { return data_.begin(); }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data_.cbegin(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data_.cbegin(); }

  [[nodiscard]] constexpr iterator end() noexcept { return std::next(data_.begin(), size_); }

  [[nodiscard]] constexpr const_iterator end() const noexcept
  {
    return std::next(data_.cbegin(), static_cast<difference_type>(size_));
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
  {
    return std::next(data_.rbegin(), static_cast<difference_type>(Capacity - size_));
  }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
  {
    return std::next(data_.crbegin(), static_cast<difference_type>(Capacity - size_));
  }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept { return data_.rend(); }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return data_.crend(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return data_.crend(); }

  template<typename Value> constexpr value_type &push_back(Value &&value)
  {
    if (size_ == Capacity) { throw std::length_error("push_back would exceed static capacity"); }
    data_[size_] = std::forward<Value>(value);
    return data_[size_++];
  }

  template<typename... Param> constexpr value_type &emplace_back(Param &&...param)
  {
    if (size_ == Capacity) { throw std::length_error("emplace_back would exceed static capacity"); }
    return data_[size_++] = data_type(std::forward<Param>(param)...);
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

  // resets the size to 0, but does not destroy any existing objects
  constexpr void clear() { size_ = 0; }


  // cppcheck-suppress functionStatic
  constexpr void reserve(size_type new_capacity)
  {
    if (new_capacity > Capacity) { throw std::length_error("new capacity would exceed max_size for stack_vector"); }
  }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type capacity() noexcept { return Capacity; }

  // cppcheck-suppress functionStatic
  [[nodiscard]] constexpr static size_type max_size() noexcept { return Capacity; }

  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }


  constexpr void resize(const size_type new_size)
  {
    if (new_size <= size_) {
      size_ = new_size;
    } else {
      if (new_size > Capacity) {
        throw std::length_error("resize would exceed static capacity");
      } else {
        auto old_end = end();
        size_ = new_size;
        auto new_end = end();
        while (old_end != new_end) {
          *old_end = data_type{};
          ++old_end;
        }
      }
    }
  }

  constexpr void pop_back() noexcept { --size_; }

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


template<typename Contained, std::size_t LHSSize, std::size_t RHSSize>
[[nodiscard]] constexpr bool operator==(const simple_stack_vector<Contained, LHSSize> &lhs, const simple_stack_vector<Contained, RHSSize> &rhs)
{
  if (lhs.size() == rhs.size()) {
    for (std::size_t idx = 0; idx < lhs.size(); ++idx) {
      if (lhs[idx] != rhs[idx]) {
        return false;
      }
    }
    return true;
  }

  return false;
}



}// namespace lefticus::tools


#endif
