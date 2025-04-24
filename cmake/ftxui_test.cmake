if (NOT FTXUI_BUILD_TESTS)
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
  src/ftxui/component/dropdown_test.cpp
  src/ftxui/component/hoverable_test.cpp
  src/ftxui/component/input_test.cpp
  src/ftxui/component/menu_test.cpp
  src/ftxui/component/modal_test.cpp
  src/ftxui/component/radiobox_test.cpp
  src/ftxui/util/ref_test.cpp
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
  src/ftxui/dom/hyperlink_test.cpp
  src/ftxui/dom/italic_test.cpp
  src/ftxui/dom/linear_gradient_test.cpp
  src/ftxui/dom/scroll_indicator_test.cpp
  src/ftxui/dom/selection_test.cpp
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
target_compile_features(ftxui-tests PRIVATE cxx_std_20)

# Disable unity build for tests. There are several files defining the same
# function in different anonymous namespaces. This is not allowed in unity
# builds, as it would result in multiple definitions of the same function.
set_target_properties(ftxui-tests PROPERTIES UNITY_BUILD OFF)

if (FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  target_compile_definitions(ftxui-tests
    PRIVATE "FTXUI_MICROSOFT_TERMINAL_FALLBACK")
endif()

include(GoogleTest)
gtest_discover_tests(ftxui-tests
  DISCOVERY_TIMEOUT 600
)

#set(CMAKE_CTEST_ARGUMENTS "--rerun-failed --output-on-failure")
#set_tests_properties(gen_init_queries PROPERTIES FIXTURES_SETUP f_init_queries)
#set_tests_properties(test PROPERTIES  FIXTURES_REQUIRED f_init_queries)
