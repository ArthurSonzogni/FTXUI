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
  GIT_TAG        62937f91b5c763a8e119d0c20c67b87bde8eff1c 
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
