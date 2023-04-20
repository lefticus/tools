include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(setup_options)
  option(tools_ENABLE_HARDENING "Enable hardening" ON)
  option(tools_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    tools_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    tools_ENABLE_HARDENING
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

  check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  option(tools_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${LIBFUZZER_SUPPORTED})


  if(NOT PROJECT_IS_TOP_LEVEL OR tools_PACKAGING_MAINTAINER_MODE)
    option(tools_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(tools_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(tools_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(tools_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(tools_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(tools_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(tools_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(tools_ENABLE_PCH "Enable precompiled headers" OFF)
    option(tools_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(tools_ENABLE_IPO "Enable IPO/LTO" ON)
    option(tools_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(tools_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(tools_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(tools_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(tools_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(tools_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(tools_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(tools_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(tools_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(tools_ENABLE_PCH "Enable precompiled headers" OFF)
    option(tools_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      tools_ENABLE_IPO
      tools_WARNINGS_AS_ERRORS
      tools_ENABLE_USER_LINKER
      tools_ENABLE_SANITIZER_ADDRESS
      tools_ENABLE_SANITIZER_LEAK
      tools_ENABLE_SANITIZER_UNDEFINED
      tools_ENABLE_SANITIZER_THREAD
      tools_ENABLE_SANITIZER_MEMORY
      tools_ENABLE_UNITY_BUILD
      tools_ENABLE_CLANG_TIDY
      tools_ENABLE_CPPCHECK
      tools_ENABLE_COVERAGE
      tools_ENABLE_PCH
      tools_ENABLE_CACHE)
  endif()
endmacro()

macro(global_options)
  if(tools_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    enable_ipo()
  endif()

  if(tools_ENABLE_HARDENING AND tools_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT tools_ENABLE_SANITIZER_UNDEFINED)
    enable_hardening(tools_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(local_options)
  include(cmake/StandardProjectSettings.cmake)

  add_library(tools_warnings INTERFACE)
  add_library(tools_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  set_project_warnings(
    tools_warnings
    ${tools_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(tools_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(tools_options)
  endif()

  include(cmake/Sanitizers.cmake)
  enable_sanitizers(
    tools_options
    ${tools_ENABLE_SANITIZER_ADDRESS}
    ${tools_ENABLE_SANITIZER_LEAK}
    ${tools_ENABLE_SANITIZER_UNDEFINED}
    ${tools_ENABLE_SANITIZER_THREAD}
    ${tools_ENABLE_SANITIZER_MEMORY})

  set_target_properties(tools_options PROPERTIES UNITY_BUILD ${tools_ENABLE_UNITY_BUILD})

  if(tools_ENABLE_PCH)
    target_precompile_headers(
      tools_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(tools_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(tools_ENABLE_CLANG_TIDY)
    enable_clang_tidy(tools_options ${tools_WARNINGS_AS_ERRORS})
  endif()

  if(tools_ENABLE_CPPCHECK)
    enable_cppcheck(${tools_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(tools_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    enable_coverage(tools_options)
  endif()

  if(tools_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(tools_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(tools_ENABLE_HARDENING AND NOT tools_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    set(ENABLE_UBSAN_MINIMAL_RUNTIME NOT tools_ENABLE_SANITIZER_UNDEFINED)
    enable_hardening(tools_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
