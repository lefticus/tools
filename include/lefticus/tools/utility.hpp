#ifndef TOOLS_UTILITY_HPP
#define TOOLS_UTILITY_HPP

// forward declare std::pair
namespace std {
template<typename LHS, typename RHS> class pair;
}

namespace lefticus::tools {
template<typename First, typename Second> struct pair
{
  First first;
  Second second;
};

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const pair<First, Second> &lhs, const pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const ::std::pair<First, Second> &lhs, const pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second>
[[nodiscard]] constexpr bool operator==(const pair<First, Second> &lhs, const ::std::pair<First, Second> &rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename First, typename Second> pair(First f, Second s) -> pair<First, Second>;
}// namespace lefticus::tools

#endif// TOOLS_UTILITY_HPP
