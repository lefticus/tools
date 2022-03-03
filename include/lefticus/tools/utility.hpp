#ifndef TOOLS_UTILITY_HPP
#define TOOLS_UTILITY_HPP

namespace lefticus::tools {
  template<typename First, typename Second>
  struct pair {
    First first;
    Second second;
  };

  template<typename First, typename Second>
  pair(First f, Second s) -> pair<First, Second>;
}

#endif// TOOLS_UTILITY_HPP
