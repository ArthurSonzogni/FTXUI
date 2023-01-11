if (NOT FTXUI_BUILD_TESTS OR
    NOT ${CMAKE_VERSION} VERSION_GREATER "3.11.4")
  return()
endif()

enable_testing()

include(cmake/ftxui_find_google_test.cmake)

add_executable(ftxui-tests
  src/ftxui/component/animation_test.cpp
  src/ftxui/component/button_test.cpp
  src/ftxui/component/collapsible_test.cpp
  src/ftxui/component/component_test.cpp
  src/ftxui/component/component_test.cpp
  src/ftxui/component/container_test.cpp
  src/ftxui/component/hoverable_test.cpp
  src/ftxui/component/input_test.cpp
  src/ftxui/component/menu_test.cpp
  src/ftxui/component/modal_test.cpp
  src/ftxui/component/radiobox_test.cpp
  src/ftxui/component/receiver_test.cpp
  src/ftxui/component/resizable_split_test.cpp
  src/ftxui/component/screen_interactive_test.cpp
  src/ftxui/component/slider_test.cpp
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

target_link_libraries(ftxui-tests
  PRIVATE component
  PRIVATE GTest::gtest
  PRIVATE GTest::gtest_main
)
target_include_directories(ftxui-tests
  PRIVATE src
)
ftxui_set_options(ftxui-tests)
target_compile_features(ftxui-tests PUBLIC cxx_std_20)

include(GoogleTest)
gtest_discover_tests(ftxui-tests
  DISCOVERY_TIMEOUT 600
)
