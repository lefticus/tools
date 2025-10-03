#include <catch2/catch.hpp>
#include <lefticus/tools/moving_ref.hpp>

#ifdef CATCH_CONFIG_RUNTIME_STATIC_REQUIRE
#define CONSTEXPR
#else
// NOLINTNEXTLINE
#define CONSTEXPR constexpr
#endif



#include <string>


struct CountsMovesCopies
{
  constexpr CountsMovesCopies() = default;
  constexpr CountsMovesCopies(CountsMovesCopies &&other) noexcept {
    ++other.move_constructed_from;
  }
  constexpr CountsMovesCopies(const CountsMovesCopies &other) noexcept {
    ++other.copy_constructed_from;
  }

  constexpr CountsMovesCopies &operator=(CountsMovesCopies &&other) noexcept {
    ++other.move_assigned_from;
    return *this;
  }

  constexpr CountsMovesCopies &operator=(const CountsMovesCopies &other) noexcept {
    ++other.copy_assigned_from;
    return *this;
  }

  mutable int move_constructed_from = 0;
  mutable int copy_constructed_from = 0;
  mutable int move_assigned_from = 0;
  mutable int copy_assigned_from = 0;

};

struct CopyOnlyType
{
  ~CopyOnlyType() = default;
};



template <typename T>
constexpr auto can_construct_ref() {
  return requires(T mct) {
    lefticus::moving_ref<T>{std::move(mct)};
  };
}

template <typename T>
constexpr auto can_construct_ref_const() {
  return requires(const T mct) {
    lefticus::moving_ref<T>{std::move(mct)};
  };
}

template <typename T>
constexpr auto can_assign() {
  return requires(T lhs, T mct) {
    lhs = lefticus::moving_ref<T>{std::move(mct)};
  };
}


template <typename T>
constexpr auto can_assign_const() {
  return requires(T lhs, const T mct) {
    lhs = lefticus::moving_ref<T>{std::move(mct)};
  };
}

template <typename T>
constexpr auto can_construct() {
  return requires(T mct) {
    T{lefticus::moving_ref<T>{std::move(mct)}};
  };
}


template <typename T>
constexpr auto can_construct_const() {
  return requires(const T mct) {
    T{lefticus::moving_ref<T>{std::move(mct)}};
  };
}



TEST_CASE("[moving_ref] behaves as expected")
{
  constexpr auto implicit_move = [](){
    CountsMovesCopies cmc;
    lefticus::moving_ref mr{std::move(cmc)};
    [[maybe_unused]] CountsMovesCopies cmc2 = mr;

    return cmc.move_constructed_from;
  };

  STATIC_REQUIRE(implicit_move() == 1);

  constexpr auto implicit_move_assign = [](){
    CountsMovesCopies cmc;
    lefticus::moving_ref mr{std::move(cmc)};
    CountsMovesCopies cmc2;
    cmc2 = mr;

    return cmc.move_assigned_from;
  };

  STATIC_REQUIRE(implicit_move_assign() == 1);

  STATIC_REQUIRE(can_construct_ref<CountsMovesCopies>());
  STATIC_REQUIRE(!can_construct_ref_const<CountsMovesCopies>());
  STATIC_REQUIRE(can_assign<CountsMovesCopies>());
  STATIC_REQUIRE(!can_assign_const<CountsMovesCopies>());
  STATIC_REQUIRE(can_construct<CountsMovesCopies>());
  STATIC_REQUIRE(!can_construct_const<CountsMovesCopies>());

  STATIC_REQUIRE(!can_construct_ref<CopyOnlyType>());
  STATIC_REQUIRE(!can_construct_ref_const<CopyOnlyType>());
  STATIC_REQUIRE(!can_assign<CopyOnlyType>());
  STATIC_REQUIRE(!can_assign_const<CopyOnlyType>());
  STATIC_REQUIRE(!can_construct<CopyOnlyType>());
  STATIC_REQUIRE(!can_construct_const<CopyOnlyType>());


}
