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

template <default_constructible Contained, std::size_t Capacity>
struct simple_stack_vector {
  using value_type = Contained;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using data_type = std::array<value_type, Capacity>;

  using iterator = data_type::iterator;
  using const_iterator = data_type::const_iterator;
  using reverse_iterator = data_type::reverse_iterator;
  using const_reverse_iterator = data_type::const_reverse_iterator;

  [[nodiscard]] constexpr iterator begin() noexcept { return data.begin(); }

  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return data.cbegin();
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return data.cbegin();
  }

  [[nodiscard]] constexpr iterator end() noexcept {
    return std::next(data.begin(), size_);
  }

  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return std::next(data.cbegin(), size_);
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr iterator rbegin() noexcept {
    return std::next(data.rbegin(), Capacity - size_);
  }

  [[nodiscard]] constexpr const_iterator rbegin() const noexcept {
    return std::next(data.crbegin(), Capacity - size_);
  }
  [[nodiscard]] constexpr const_iterator crbegin() const noexcept {
    return crbegin();
  }

  [[nodiscard]] constexpr bool empty() noexcept { return size_ == 0; }

  [[nodiscard]] constexpr iterator rend() noexcept { return data.rend(); }

  [[nodiscard]] constexpr const_iterator rend() const noexcept {
    return data.crend();
  }

  [[nodiscard]] constexpr const_iterator crend() const noexcept {
    return data.crend();
  }

  template<typename Value>
  [[nodiscard]] constexpr data_type &push_back(Value &&value) {
    if (size_ == Capacity) {
      throw std::length_error("push_back would exceed static capacity");
    }
    return data[size_++] = std::forward<Value>(value);
  }

  template<typename ... Param>
  [[nodiscard]] constexpr data_type &emplace_back(Param  && ... param) {
    if (size_ == Capacity) {
      throw std::length_error("emplace_back would exceed static capacity");
    }
    return data[size_++] = data_type(std::forward<Param>(param)...);
  }

  [[nodiscard]] constexpr data_type &operator[](const std::size_t idx) noexcept {
    return data[idx];
  }

  [[nodiscard]] constexpr const data_type &operator[](const std::size_t idx) const noexcept {
    return data[idx];
  }

  [[nodiscard]] constexpr data_type &at(const std::size_t idx) {
    if (idx > size_) {
      throw std::out_of_range("index past end of stack_vector");
    }
    return data[idx];
  }

  [[nodiscard]] constexpr const data_type &at(const std::size_t idx) const {
    if (idx > size_) {
      throw std::out_of_range("index past end of stack_vector");
    }
    return data[idx];
  }

  // resets the size to 0, but does not destroy any existing objects
  constexpr void clear() {
    size = 0;
  }

  constexpr void reserve(size_type new_capacity) {
    if (new_capacity > Capacity) {
      throw std::length_error("new capacity would exceed max_size for stack_vector");
    }
  }

  [[nodiscard]] constexpr static size_type capacity() noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr static size_type max_size() noexcept {
    return Capacity;
  }

  [[nodiscard]] constexpr size_type size() const noexcept {
    return size_;
  }

  constexpr void resize(const size_type new_size) {
    if (new_size <= size_) {
      size = new_size;
    } else {
      if (new_size > Capacity) {
      throw std::length_error("resize would exceed static capacity");
      } else {
        auto old_end = end();
        size = new_size;
        auto new_end = end();
        while (old_end != new_end) {
          *old_end = data_type{};
          ++old_end;
        }
      }
    }
  }

  constexpr void pop_back() noexcept {
    --size_;
  }

  constexpr void shrink_to_fit() noexcept {
    // nothing to do here
  }



 private:
  data_type data;
  size_type size_{};
};
}


#endif
