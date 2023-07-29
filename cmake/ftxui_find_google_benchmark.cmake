# Some developers would be happier with the google benchmark version provided
# from their package manager. Use it if it is installed the package provide
# cmake support.
# https://github.com/ArthurSonzogni/FTXUI/issues/551
find_package(benchmark QUIET)
if (benchmark_FOUND)
  return()
endif()

option(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
option(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(googlebenchmark
  GIT_REPOSITORY "https://github.com/google/benchmark"
  GIT_TAG        "015d1a091af6937488242b70121858bce8fd40e9"  # v1.8.2
  GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(googlebenchmark)
set (BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
set (BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
if(googlebenchmark_POPULATED)
  return()
endif()

FetchContent_Populate(googlebenchmark)
add_subdirectory(
  ${googlebenchmark_SOURCE_DIR}
  ${googlebenchmark_BINARY_DIR}
  EXCLUDE_FROM_ALL
)
