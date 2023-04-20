include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(lefticus_tools_setup_options)
  option(lefticus_tools_ENABLE_HARDENING "Enable hardening" ON)
  option(lefticus_tools_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    lefticus_tools_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    lefticus_tools_ENABLE_HARDENING
    OFF)


  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()

  lefticus_tools_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  option(lefticus_tools_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${LIBFUZZER_SUPPORTED})


  if(NOT PROJECT_IS_TOP_LEVEL OR lefticus_tools_PACKAGING_MAINTAINER_MODE)
    option(lefticus_tools_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(lefticus_tools_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(lefticus_tools_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(lefticus_tools_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(lefticus_tools_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(lefticus_tools_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(lefticus_tools_ENABLE_PCH "Enable precompiled headers" OFF)
    option(lefticus_tools_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(lefticus_tools_ENABLE_IPO "Enable IPO/LTO" ON)
    option(lefticus_tools_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(lefticus_tools_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(lefticus_tools_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(lefticus_tools_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(lefticus_tools_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(lefticus_tools_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(lefticus_tools_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(lefticus_tools_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(lefticus_tools_ENABLE_PCH "Enable precompiled headers" OFF)
    option(lefticus_tools_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      lefticus_tools_ENABLE_IPO
      lefticus_tools_WARNINGS_AS_ERRORS
      lefticus_tools_ENABLE_USER_LINKER
      lefticus_tools_ENABLE_SANITIZER_ADDRESS
      lefticus_tools_ENABLE_SANITIZER_LEAK
      lefticus_tools_ENABLE_SANITIZER_UNDEFINED
      lefticus_tools_ENABLE_SANITIZER_THREAD
      lefticus_tools_ENABLE_SANITIZER_MEMORY
      lefticus_tools_ENABLE_UNITY_BUILD
      lefticus_tools_ENABLE_CLANG_TIDY
      lefticus_tools_ENABLE_CPPCHECK
      lefticus_tools_ENABLE_COVERAGE
      lefticus_tools_ENABLE_PCH
      lefticus_tools_ENABLE_CACHE)
  endif()
endmacro()

macro(lefticus_tools_global_options)
  if(lefticus_tools_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    lefticus_tools_enable_ipo()
  endif()

  if(lefticus_tools_ENABLE_HARDENING AND lefticus_tools_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT lefticus_tools_ENABLE_SANITIZER_UNDEFINED)
    lefticus_tools_enable_hardening(lefticus_tools_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(lefticus_tools_local_options)
  include(cmake/StandardProjectSettings.cmake)

  add_library(lefticus_tools_warnings INTERFACE)
  add_library(lefticus_tools_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  lefticus_tools_set_project_warnings(
    lefticus_tools_warnings
    ${lefticus_tools_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(lefticus_tools_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    lefticus_tools_configure_linker(lefticus_tools_options)
  endif()

  include(cmake/Sanitizers.cmake)
  lefticus_tools_enable_sanitizers(
    lefticus_tools_options
    ${lefticus_tools_ENABLE_SANITIZER_ADDRESS}
    ${lefticus_tools_ENABLE_SANITIZER_LEAK}
    ${lefticus_tools_ENABLE_SANITIZER_UNDEFINED}
    ${lefticus_tools_ENABLE_SANITIZER_THREAD}
    ${lefticus_tools_ENABLE_SANITIZER_MEMORY})

  set_target_properties(lefticus_tools_options PROPERTIES UNITY_BUILD ${lefticus_tools_ENABLE_UNITY_BUILD})

  if(lefticus_tools_ENABLE_PCH)
    target_precompile_headers(
      lefticus_tools_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(lefticus_tools_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    lefticus_tools_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(lefticus_tools_ENABLE_CLANG_TIDY)
    lefticus_tools_enable_clang_tidy(lefticus_tools_options ${lefticus_tools_WARNINGS_AS_ERRORS})
  endif()

  if(lefticus_tools_ENABLE_CPPCHECK)
    lefticus_tools_enable_cppcheck(${lefticus_tools_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(lefticus_tools_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    lefticus_tools_enable_coverage(lefticus_tools_options)
  endif()

  if(lefticus_tools_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(lefticus_tools_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(lefticus_tools_ENABLE_HARDENING AND NOT lefticus_tools_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT lefticus_tools_ENABLE_SANITIZER_UNDEFINED)
    lefticus_tools_enable_hardening(lefticus_tools_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
