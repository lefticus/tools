#ifndef LEFTICUS_TOOLS_MOVING_REF_HPP
#define LEFTICUS_TOOLS_MOVING_REF_HPP

#include <type_traits>
#include <utility>

namespace lefticus {


#ifdef __cpp_concepts
template<class P>
struct DetectMoveConstruction
{
    constexpr operator P const&();
    constexpr operator P&&();
};

template<typename T>
concept copyable_xor_moveable = requires (T t, DetectMoveConstruction<T> m) {
  // borrowed from https://stackoverflow.com/questions/51901837/how-to-get-if-a-type-is-truly-move-constructible/51912859#51912859
  // if this line below compiles then we know we only have either
  // a move assignment or a copy assignment, otherwise
  // it would be ambiguous
  // we cannot detect the constructor because MSVC
  // seems to have a bug, but we can detect assignment!
  t = m;
};

template<typename T>
concept has_move_ctor = 
  std::move_constructible<T> && !copyable_xor_moveable<T>;
#endif



#ifdef __cpp_concepts
template<has_move_ctor T>
#else
template<typename T>
#endif

struct moving_ref {
    constexpr moving_ref(T &&val) : ref{std::move(val)} {}
    constexpr operator T&&() {
        return std::move(ref);
    }
    T &&ref;
};

}

#endif

