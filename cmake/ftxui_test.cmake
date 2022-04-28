enable_testing()

option(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
option(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        23ef29555ef4789f555f1ba8c51b4c52975f0907
  GIT_PROGRESS   TRUE
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  set(BUILD_GMOCK OFF CACHE INTERNAL "")
  set(INSTALL_GTEST OFF CACHE INTERNAL "")
  set(gtest_force_shared_crt ON CACHE INTERNAL "")
  add_subdirectory(
    ${googletest_SOURCE_DIR}
    ${googletest_BINARY_DIR}
    EXCLUDE_FROM_ALL
  )
endif()

add_executable(tests
  src/ftxui/component/animation_test.cpp
  src/ftxui/component/button_test.cpp
  src/ftxui/component/collapsible_test.cpp
  src/ftxui/component/component_test.cpp
  src/ftxui/component/component_test.cpp
  src/ftxui/component/container_test.cpp
  src/ftxui/component/input_test.cpp
  src/ftxui/component/menu_test.cpp
  src/ftxui/component/radiobox_test.cpp
  src/ftxui/component/receiver_test.cpp
  src/ftxui/component/resizable_split_test.cpp
  src/ftxui/component/screen_interactive_test.cpp
  src/ftxui/component/terminal_input_parser_test.cpp
  src/ftxui/component/toggle_test.cpp
  src/ftxui/dom/blink_test.cpp
  src/ftxui/dom/bold_test.cpp
  src/ftxui/dom/border_test.cpp
  src/ftxui/dom/canvas_test.cpp
  src/ftxui/dom/color_test.cpp
  src/ftxui/dom/dbox_test.cpp
  src/ftxui/dom/dim_test.cpp
  src/ftxui/dom/flexbox_helper_test.cpp
  src/ftxui/dom/flexbox_test.cpp
  src/ftxui/dom/gauge_test.cpp
  src/ftxui/dom/gridbox_test.cpp
  src/ftxui/dom/hbox_test.cpp
  src/ftxui/dom/scroll_indicator_test.cpp
  src/ftxui/dom/separator_test.cpp
  src/ftxui/dom/spinner_test.cpp
  src/ftxui/dom/table_test.cpp
  src/ftxui/dom/text_test.cpp
  src/ftxui/dom/underlined_test.cpp
  src/ftxui/dom/vbox_test.cpp
  src/ftxui/screen/color_test.cpp
  src/ftxui/screen/string_test.cpp
)

target_link_libraries(tests
  PRIVATE component
  PRIVATE gtest
  PRIVATE gtest_main
)
target_include_directories(tests
  PRIVATE src
)
ftxui_set_options(tests)

include(GoogleTest)
gtest_discover_tests(tests
  DISCOVERY_TIMEOUT 600
)


include(cmake/ftxui_benchmark.cmake)

if (FTXUI_BUILD_TESTS_FUZZER)
  include(cmake/ftxui_fuzzer.cmake)
endif()
