enable_testing()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
include(FetchContent)

FetchContent_Declare( googletest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        23ef29555ef4789f555f1ba8c51b4c52975f0907
)
FetchContent_MakeAvailable(googletest)

add_executable(tests
  src/ftxui/component/component_test.cpp
  src/ftxui/component/container_test.cpp
  src/ftxui/component/input_test.cpp
  src/ftxui/component/radiobox_test.cpp
  src/ftxui/component/receiver_test.cpp
  src/ftxui/component/screen_interactive_test.cpp
  src/ftxui/component/terminal_input_parser_test.cpp
  src/ftxui/component/toggle_test.cpp
  src/ftxui/dom/gauge_test.cpp
  src/ftxui/dom/table_test.cpp
  src/ftxui/dom/gridbox_test.cpp
  src/ftxui/dom/hbox_test.cpp
  src/ftxui/dom/text_test.cpp
  src/ftxui/dom/vbox_test.cpp
  src/ftxui/screen/string_test.cpp
)

target_link_libraries(tests
  PRIVATE component
  PRIVATE gtest
  PRIVATE gmock
  PRIVATE gtest_main
)
target_include_directories(tests
  PRIVATE src
)
target_compile_options(tests PRIVATE -fsanitize=address)
target_link_libraries(tests PRIVATE -fsanitize=address)

if (NOT MSVC)
  include(cmake/ftxui_benchmark.cmake)
endif()

if (FTXUI_BUILD_TESTS_FUZZER)
  include(cmake/ftxui_fuzzer.cmake)
endif()
