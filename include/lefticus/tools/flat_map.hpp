#ifndef LEFTICUS_TOOLS_FLAT_MAP_HPP
#define LEFTICUS_TOOLS_FLAT_MAP_HPP

#include <map>
#include <vector>

namespace lefticus::tools {

template<typename Key, typename Value, typename Container = std::vector<std::pair<const Key, Value>>> struct flat_map
{
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using reverse_iterator = typename Container::reverse_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;

  using mapped_type = Value;
  using key_type = Key;
  using difference_type = typename Container::difference_type;

  using value_type = std::pair<const Key, Value>;
  using reference = value_type &;
  using const_reference = const value_type &;

  // static_assert(std::is_same_v<value_type, typename Container::value_type>);

  [[nodiscard]] constexpr iterator begin() noexcept { return data.begin(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data.begin(); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return data.cbegin(); }

  [[nodiscard]] constexpr iterator end() noexcept { return data.end(); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return data.end(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return data.cend(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return data.rbegin(); }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return data.begin(); }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return data.crbegin(); }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept { return data.rend(); }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return data.rend(); }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return data.crend(); }

  [[nodiscard]] constexpr mapped_type &operator[](const key_type &key) { return this->try_emplace(key).first->second; }

  [[nodiscard]] constexpr mapped_type &operator[](key_type &&key)
  {
    return this->try_emplace(std::move(key)).first->second;
  }

  template<typename K, typename This> [[nodiscard]] constexpr static auto find(const K &k, This *obj)
  {
    auto itr = obj->data.begin();
    for (; itr != obj->data.end(); ++itr) {
      if (itr->first == k) { return itr; }
    }
    return itr;
  }

  template<typename K> [[nodiscard]] constexpr const_iterator find(const K &k) const { return find(k, this); }

  template<typename K> [[nodiscard]] constexpr iterator find(const K &k) { return find(k, this); }

  template<typename K, typename This> [[nodiscard]] static constexpr auto &at(const K &k, This *obj)
  {
    const auto itr = obj->find(k);
    if (itr != obj->data.end()) { return itr->second; }
    throw std::out_of_range("Key not found");
  }

  template<typename K> [[nodiscard]] constexpr mapped_type &at(const K &k) { return at(k, this); }

  template<typename K> [[nodiscard]] constexpr const mapped_type &at(const K &k) const { return at(k, this); }

  template<class K, class... Args> constexpr std::pair<iterator, bool> try_emplace(K &&k, Args &&...args)
  {
    auto found = find(k);
    if (found != data.end()) { return { found, false }; }

    data.emplace_back(value_type{ std::forward<K>(k), { std::forward<Args>(args)... } });
    return { std::next(data.begin(), static_cast<difference_type>(data.size() - 1)), true };
  }

  Container data;
};
}// namespace lefticus::tools

#endif

