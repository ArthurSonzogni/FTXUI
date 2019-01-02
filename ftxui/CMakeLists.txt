cmake_minimum_required(VERSION 3.0)

add_library(ftxui
  src/ftxui/component/component.cpp
  src/ftxui/component/component_direction.cpp
  src/ftxui/component/component_horizontal.cpp
  src/ftxui/component/component_vertical.cpp
  src/ftxui/component/input.cpp
  src/ftxui/component/menu.cpp
  src/ftxui/component/toggle.cpp
  src/ftxui/dom/blink.cpp
  src/ftxui/dom/bold.cpp
  src/ftxui/dom/color.cpp
  src/ftxui/dom/composite_decorator.cpp
  src/ftxui/dom/dbox.cpp
  src/ftxui/dom/dim.cpp
  src/ftxui/dom/flex.cpp
  src/ftxui/dom/frame.cpp
  src/ftxui/dom/gauge.cpp
  src/ftxui/dom/hbox.cpp
  src/ftxui/dom/inverted.cpp
  src/ftxui/dom/node.cpp
  src/ftxui/dom/node_decorator.cpp
  src/ftxui/dom/separator.cpp
  src/ftxui/dom/text.cpp
  src/ftxui/dom/underlined.cpp
  src/ftxui/dom/util.cpp
  src/ftxui/dom/vbox.cpp
  src/ftxui/event.cpp
  src/ftxui/screen.cpp
  src/ftxui/screen_interactive.cpp
  src/ftxui/terminal.cpp
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
    src/ftxui/dom/gauge_test.cpp
    src/ftxui/dom/hbox_test.cpp
    src/ftxui/dom/text_test.cpp
    src/ftxui/dom/vbox_test.cpp
  )
endif()
