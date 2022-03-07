find_package(Catch2 REQUIRED)

include(Catch)

add_library(catch_main OBJECT catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE project_options)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE project_warnings project_options catch_main)

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
    constexpr_tests
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
  target_link_libraries("constexpr_${test_name}" PRIVATE project_options project_warnings catch_main)
  discover_tests("constexpr_${test_name}" "constexpr")

  # Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
  # things go wrong with the constexpr testing
  add_executable("relaxed_constexpr_${test_name}" ${ARGN})
  target_link_libraries("relaxed_constexpr_${test_name}" PRIVATE project_options project_warnings catch_main)
  target_compile_definitions("relaxed_constexpr_${test_name}" PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
  discover_tests("relaxed_constexpr_${test_name}" "relaxed_constexpr")
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
  )
