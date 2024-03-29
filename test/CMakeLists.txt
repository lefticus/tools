cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(tools CONFIG REQUIRED) # for intro, lefticus::tools_options, ...

  if(NOT TARGET lefticus::tools_options)
    message(FATAL_ERROR "Requiered config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)
add_executable(tests tests.cpp ../include/lefticus/tools/utility.hpp)

add_library(catch_main OBJECT catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE lefticus::tools_options)

target_link_libraries(
  tests
  PRIVATE lefticus::tools
          lefticus::tools_warnings
          lefticus::tools_options
          catch_main)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

function(discover_tests test_name prefix)
  catch_discover_tests(
    ${test_name}
    TEST_PREFIX
    "${prefix}."
    REPORTER
    xml
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "${prefix}."
    OUTPUT_SUFFIX
    .xml)
endfunction()

function(add_constexpr_test_executables test_name)
  add_executable("constexpr_${test_name}" ${ARGN})
  discover_tests("constexpr_${test_name}" "constexpr_${test_name}")

  # Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
  # things go wrong with the constexpr testing
  add_executable("relaxed_constexpr_${test_name}" ${ARGN})
  target_compile_definitions("relaxed_constexpr_${test_name}" PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
  discover_tests("relaxed_constexpr_${test_name}" "relaxed_constexpr_${test_name}")
endfunction()

add_constexpr_test_executables(
  tests
  consteval_invoke.cpp
  curry_tests.cpp
  lambda_coroutine_tests.cpp
  np_tests.cpp
  simple_stack_vector_tests.cpp
  static_views_tests.cpp
  simple_stack_string_tests.cpp
  flat_map_tests.cpp
  type_lists_tests.cpp
  strong_types_tests.cpp)
target_link_libraries(
  "constexpr_tests"
  PRIVATE lefticus::tools
          lefticus::tools_options
          lefticus::tools_warnings
          catch_main)
target_link_libraries(
  "relaxed_constexpr_tests"
  PRIVATE lefticus::tools
          lefticus::tools_options
          lefticus::tools_warnings
          catch_main)

add_library(cpp17_catch_main OBJECT catch_main.cpp)
target_link_libraries(cpp17_catch_main PUBLIC Catch2::Catch2)

add_constexpr_test_executables(
  cpp17_tests
  simple_stack_vector_tests.cpp
  simple_stack_string_tests.cpp
  flat_map_tests.cpp)

target_include_directories(constexpr_cpp17_tests PRIVATE ../include)
target_include_directories(relaxed_constexpr_cpp17_tests PRIVATE ../include)
target_link_libraries("constexpr_cpp17_tests" PRIVATE lefticus::tools lefticus::tools_warnings cpp17_catch_main)
target_link_libraries("relaxed_constexpr_cpp17_tests" PRIVATE lefticus::tools lefticus::tools_warnings cpp17_catch_main)

set_property(TARGET cpp17_catch_main constexpr_cpp17_tests relaxed_constexpr_cpp17_tests PROPERTY CXX_STANDARD 17)

function(test_header_compiles header_name)
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${header_name}_compile_test.cpp" "#include <lefticus/tools/${header_name}>")
  add_library("${header_name}_compile_test" STATIC "${CMAKE_CURRENT_BINARY_DIR}/${header_name}_compile_test.cpp")
  target_link_libraries("${header_name}_compile_test" PRIVATE lefticus::tools lefticus::tools_warnings lefticus::tools_options)
endfunction()

test_header_compiles(simple_stack_string.hpp)
test_header_compiles(simple_stack_vector.hpp)
test_header_compiles(consteval_invoke.hpp)
test_header_compiles(curry.hpp)
test_header_compiles(flat_map.hpp)
test_header_compiles(flat_map_adapter.hpp)
test_header_compiles(lambda_coroutines.hpp)
test_header_compiles(non_promoting_ints.hpp)
test_header_compiles(simple_stack_flat_map.hpp)
test_header_compiles(static_views.hpp)
test_header_compiles(utility.hpp)
test_header_compiles(strong_types.hpp)
test_header_compiles(type_lists.hpp)
