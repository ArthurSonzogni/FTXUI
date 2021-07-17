if (NOT WIN32)
  FetchContent_Declare(googlebenchmark
    GIT_REPOSITORY "https://github.com/google/benchmark"
    GIT_TAG 62937f91b5c763a8e119d0c20c67b87bde8eff1c 
  )
  FetchContent_MakeAvailable(googlebenchmark)

  add_executable(ftxui_benchmark
    src/ftxui/dom/benchmark_test.cpp
  )
  target_link_libraries(ftxui_benchmark
    PRIVATE dom
    PRIVATE benchmark::benchmark
    PRIVATE benchmark::benchmark_main
  )
  target_include_directories(ftxui_benchmark
    PRIVATE src
  )
endif()
