#ifndef LEFTICUS_TOOLS_POLYMORPHIC_VARIANT_HPP
#define LEFTICUS_TOOLS_POLYMORPHIC_VARIANT_HPP

namespace lefticus::tools {
template<typename Base, typename... Type> const Base *get_base(const std::variant<Type...> &input)
{
  const Base *retval = nullptr;

  ((retval = retval ? retval : std::get_if<Type>(&input)), ...);

  return retval;
}

template<typename Base, typename... Type_List> Base *get_base(std::variant<Type_List...> &input)
{
  Base *retval = nullptr;

  ((retval = retval ? retval : std::get_if<Type_List>(&input)), ...);

  return retval;
}

// a variant-based type that allows polymorphic access to the underlying
// data
template<typename Base, typename... Derived>
requires std::is_polymorphic_v<Base> &&(std::is_base_of_v<Base, Derived> &&...)
  && (std::is_final_v<Derived> && ...) class polymorphic_variant
{
public:
  polymorphic_variant() = delete;

  using concrete_Type_List = Type_List<Derived...>;
  using contained_type = decltype(variant_type(concrete_Type_List{}));

  // allow any of the non-abstract supported Type_List in
  template<typename Param>
  constexpr explicit polymorphic_variant(
    Param &&p) requires contains_type_v<std::remove_cvref_t<Param>, concrete_Type_List>
    : value{ std::forward<Param>(p) }
  {}

  // anything that isn't an exact match for one of the supported Type_List
  // is deleted. Why? We don't want to slice!
  template<typename Param> constexpr explicit polymorphic_variant(Param &&) = delete;

  template<typename T, typename... Args>
  constexpr explicit polymorphic_variant(std::in_place_type_t<T> ipt, Args &&...args)
    : value(ipt, std::forward<Args>(args)...)
  {}

  [[nodiscard]] constexpr Base *get() { return get_base<Base>(value); }
  [[nodiscard]] constexpr const Base *get() const { return get_base<Base>(value); }

  [[nodiscard]] constexpr Base &operator*() { return *get(); }
  [[nodiscard]] constexpr const Base &operator*() const { return *get(); }

  [[nodiscard]] constexpr Base *operator->() { return get(); }
  [[nodiscard]] constexpr const Base *operator->() const { return get(); }

private:
  [[nodiscard]] constexpr static auto getter() noexcept
  {
    return [](auto &obj) -> Base * { return &obj; };
  }

  [[nodiscard]] constexpr static auto const_getter() noexcept
  {
    return [](const auto &obj) -> const Base * { return &obj; };
  }

  contained_type value;
};
}// namespace lefticus::tools
#endif
