//
// Created by jason on 3/12/24.
//


#include <lefticus/tools/type_lists.hpp>

static_assert(std::is_same_v<int, nth_t<2, type_list<float, double, int>>>);
static_assert(std::is_same_v<double, nth_t<1, type_list<float, double, int>>>);
static_assert(std::is_same_v<float, nth_t<0, type_list<float, double, int>>>);

static_assert(std::is_same_v<double, nth_t<0, type_list<double>>>);

static_assert(
  std::is_same_v<type_pair<type_list<>, type_list<float, double, int>>, split_n_t<0, type_list<float, double, int>>>);

static_assert(
  std::is_same_v<type_pair<type_list<float>, type_list<double, int>>, split_n_t<1, type_list<float, double, int>>>);

static_assert(
  std::is_same_v<type_pair<type_list<float, double>, type_list<int>>, split_n_t<2, type_list<float, double, int>>>);

static_assert(
  std::is_same_v<type_pair<type_list<float, double, int>, type_list<>>, split_n_t<3, type_list<float, double, int>>>);

static_assert(std::is_same_v<type_list<float, double, int>, join_t<type_list<>, type_list<float, double, int>>>);

static_assert(std::is_same_v<type_list<float, double, int>, join_t<type_list<float>, type_list<double, int>>>);
static_assert(std::is_same_v<type_list<float, double, int>, join_t<type_list<float, double>, type_list<int>>>);

static_assert(std::is_same_v<type_list<float, double, int>, join_t<type_list<float, double, int>, type_list<>>>);

static_assert(std::is_same_v<type_list<>, sub_t<0, 0, type_list<float, double, int>>>);
static_assert(std::is_same_v<type_list<float>, sub_t<0, 1, type_list<float, double, int>>>);
static_assert(std::is_same_v<type_list<double, int>, sub_t<1, 2, type_list<float, double, int>>>);

// static_assert(std::is_same_v<double, first_type<double>>);
