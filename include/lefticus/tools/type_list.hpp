#ifndef LEFTICUS_TOOLS_TYPE_LIST_HPP
#define LEFTICUS_TOOLS_TYPE_LIST_HPP

namespace lefticus::tools {

template <typename... T>
struct Type_List {};

template <typename Filter, typename First, typename... Rest, typename... Result>
constexpr auto filter_impl(Filter filter, Type_List<First, Rest...>,
                           Type_List<Result...> result) {
  if constexpr (filter.template operator()<First>()) {
    if constexpr (sizeof...(Rest) > 0) {
      return filter_impl(filter, Type_List<Rest...>{}, Type_List<Result..., First>{});
    } else {
      return Type_List<Result..., First>{};
    }
  } else {
    if constexpr (sizeof...(Rest) > 0) {
      return filter_impl(filter, Type_List<Rest...>{}, result);
    } else {
      return result;
    }
  }
}

template <typename Filter, typename First, typename... Rest>
constexpr auto filter(Filter filter, Type_List<First, Rest...> input) {
  return filter_impl(filter, input, Type_List<>{});
}

template <typename... Type>
constexpr std::variant<Type...> variant_type(Type_List<Type...>);


template <typename T, typename... Args>
constexpr inline bool contains_type_v = (std::is_same_v<T, Args> || ...);

template <typename T, typename... Type>
constexpr inline bool contains_type_v<T, Type_List<Type...>> =
    (std::is_same_v<T, Type> || ...);


}


#endif
