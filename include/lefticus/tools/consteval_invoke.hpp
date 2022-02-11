#ifndef LEFTICUS_TOOLS_CONSTEVAL_INVOKE_HPP
#define LEFTICUS_TOOLS_CONSTEVAL_INVOKE_HPP

namespace lefticus::tools {

/// Trivial wrapper around `std::invoke` to forced a function to be executed at compile-time
template<typename... Param> consteval decltype(auto) consteval_invoke(Param &&...param)
{
  return std::invoke(std::forward<Param>(param)...);
}

}// namespace lefticus::tools

#endif
