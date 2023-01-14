if (NOT FTXUI_BUILD_TESTS OR
    NOT ${CMAKE_VERSION} VERSION_GREATER "3.11.4" OR
    WIN32
    )
  return()
endif()

include(cmake/ftxui_find_google_benchmark.cmake)

add_executable(ftxui-benchmark
  src/ftxui/dom/benchmark_test.cpp
  )
ftxui_set_options(ftxui-benchmark)
target_link_libraries(ftxui-benchmark
  PRIVATE dom
  PRIVATE benchmark::benchmark
  PRIVATE benchmark::benchmark_main
  )
target_include_directories(ftxui-benchmark
  PRIVATE src
  )
