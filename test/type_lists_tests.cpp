#include <catch2/catch.hpp>
#include <lefticus/tools/type_lists.hpp>


TEST_CASE("Test nth type getter")
{
  STATIC_REQUIRE(std::is_same_v<int, lefticus::tools::nth_t<2, lefticus::tools::type_list<float, double, int>>>);
  STATIC_REQUIRE(std::is_same_v<double, lefticus::tools::nth_t<1, lefticus::tools::type_list<float, double, int>>>);
  STATIC_REQUIRE(std::is_same_v<float, lefticus::tools::nth_t<0, lefticus::tools::type_list<float, double, int>>>);

  STATIC_REQUIRE(std::is_same_v<double, lefticus::tools::nth_t<0, lefticus::tools::type_list<double>>>);
}


TEST_CASE("Test split_n type list splitter")
{
  STATIC_REQUIRE(std::is_same_v<
    lefticus::tools::type_pair<lefticus::tools::type_list<>, lefticus::tools::type_list<float, double, int>>,
    lefticus::tools::split_n_t<0, lefticus::tools::type_list<float, double, int>>>);

  STATIC_REQUIRE(std::is_same_v<
    lefticus::tools::type_pair<lefticus::tools::type_list<float>, lefticus::tools::type_list<double, int>>,
    lefticus::tools::split_n_t<1, lefticus::tools::type_list<float, double, int>>>);

  STATIC_REQUIRE(std::is_same_v<
    lefticus::tools::type_pair<lefticus::tools::type_list<float, double>, lefticus::tools::type_list<int>>,
    lefticus::tools::split_n_t<2, lefticus::tools::type_list<float, double, int>>>);

  STATIC_REQUIRE(std::is_same_v<
    lefticus::tools::type_pair<lefticus::tools::type_list<float, double, int>, lefticus::tools::type_list<>>,
    lefticus::tools::split_n_t<3, lefticus::tools::type_list<float, double, int>>>);
}


TEST_CASE("Test join type list joiner")
{
  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<float, double, int>,
    lefticus::tools::join_t<lefticus::tools::type_list<>, lefticus::tools::type_list<float, double, int>>>);

  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<float, double, int>,
    lefticus::tools::join_t<lefticus::tools::type_list<float>, lefticus::tools::type_list<double, int>>>);
  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<float, double, int>,
    lefticus::tools::join_t<lefticus::tools::type_list<float, double>, lefticus::tools::type_list<int>>>);

  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<float, double, int>,
    lefticus::tools::join_t<lefticus::tools::type_list<float, double, int>, lefticus::tools::type_list<>>>);
}

TEST_CASE("Test sub type list sub list splitter")
{
  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<>,
    lefticus::tools::sub_t<0, 0, lefticus::tools::type_list<float, double, int>>>);
  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<float>,
    lefticus::tools::sub_t<0, 1, lefticus::tools::type_list<float, double, int>>>);
  STATIC_REQUIRE(std::is_same_v<lefticus::tools::type_list<double, int>,
    lefticus::tools::sub_t<1, 2, lefticus::tools::type_list<float, double, int>>>);
}
