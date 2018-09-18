cmake_minimum_required(VERSION 3.0)
project(ftxui)

add_library(ftxui
  src/ftxui/core/component.cpp
  src/ftxui/core/dom/flex.cpp
  src/ftxui/core/dom/hbox.cpp
  src/ftxui/core/dom/node.cpp
  src/ftxui/core/dom/text.cpp
  src/ftxui/core/dom/vbox.cpp
  src/ftxui/core/screen.cpp
  src/ftxui/core/terminal.cpp
  src/ftxui/util/string.cpp
)


target_include_directories(ftxui
  PUBLIC include
  PRIVATE src
)

target_compile_features(ftxui PUBLIC cxx_std_17)
target_compile_options(ftxui PRIVATE -Wall)

# Note: For gtest, please follow:
# https://stackoverflow.com/questions/24295876/cmake-cannot-find-a-googletest-required-library
find_package(GTest)
find_package(Threads)
if (GTEST_FOUND AND THREADS_FOUND)
  function(add_new_test test_name test_files)
    add_executable(${ARGV})
    target_link_libraries(${test_name}
      PRIVATE
        ftxui
        Threads::Threads
        ${GTEST_BOTH_LIBRARIES}
    )
    target_include_directories(ftxui
      PRIVATE
        ${GTest_INCLUDE_DIRS}
        ftxui
    )
    gtest_discover_tests(${test_name})
    add_test(${test_name} ${test_name})
  endfunction(add_new_test)

  add_new_test(dom_tests
    src/ftxui/core/dom/text_test.cpp
    src/ftxui/core/dom/hbox_test.cpp
    src/ftxui/core/dom/vbox_test.cpp
  )
endif()
