//
// Created by jason on 3/12/24.
//

#ifndef LEFTICUS_TOOLS_TYPE_LISTS_HPP
#define LEFTICUS_TOOLS_TYPE_LISTS_HPP


#include <utility>

template<typename First, typename Second> struct type_pair
{
  using first_type = First;
  using second_type = Second;
};

template<typename... T> struct type_list
{
};

template<> struct type_list<>
{
};

template<typename First, typename... T> struct type_list<First, T...>
{
  using first_type = First;
};

template<typename... T> auto last_type(type_list<T...>) -> decltype((type_list<T>{}, ...))::first_type;

template<typename First, typename... T> auto first_type(type_list<First, T...>) -> First;

template<std::size_t N, typename First, typename... T>
auto nth_type_helper(type_list<First, T...>) -> First
  requires(N == 0);

// This is an optimization if we detect it's just the last one, so we don't
// recurse further
template<std::size_t N, typename... T>
auto nth_type_helper(type_list<T...> list) -> decltype(last_type(list))
  requires(N > 0 && N == sizeof...(T) - 1);

template<std::size_t N, typename First, typename... T>
auto nth_type_helper(type_list<First, T...> list) -> decltype(nth_type_helper<N - 1>(type_list<T...>{}))
  requires(N > 0 && N != sizeof...(T));

template<std::size_t N, typename... T> auto nth_type(type_list<T...> list) -> decltype(nth_type_helper<N>(list));

template<std::size_t N, typename T> using nth_t = decltype(nth_type<N>(T{}));

template<std::size_t N, typename... Last, typename... T>
auto split_n_helper(type_list<Last...> last, type_list<T...>) -> type_pair<type_list<Last...>, type_list<T...>>
  requires(N == 0);

template<std::size_t N, typename First, typename... Last, typename... T>
auto split_n_helper(type_list<Last...>, type_list<First, T...>)
  -> decltype(split_n_helper<N - 1>(type_list<Last..., First>{}, type_list<T...>{}))
  requires(N > 0);

template<std::size_t N, typename... T>
auto split_n(type_list<T...> list) -> decltype(split_n_helper<N>(type_list<>{}, list));

template<std::size_t N, typename T> using split_n_t = decltype(split_n<N>(T{}));

template<typename... LHS, typename... RHS> auto join(type_list<LHS...>, type_list<RHS...>) -> type_list<LHS..., RHS...>;

template<typename LHS, typename RHS> using join_t = decltype(join(LHS{}, RHS{}));

template<std::size_t Start, std::size_t Count, typename... T>
auto sub(type_list<T...>) ->
  typename split_n_t<Count, typename split_n_t<Start, type_list<T...>>::second_type>::first_type;

template<std::size_t Start, std::size_t Count, typename T> using sub_t = decltype(sub<Start, Count>(T{}));





#endif// LEFTICUS_TOOLS_TYPE_LISTS_HPP
