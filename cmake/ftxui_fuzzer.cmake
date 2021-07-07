set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
add_executable(terminal_input_parser_test_fuzz
  src/ftxui/component/terminal_input_parser_test_fuzz.cpp
)
target_include_directories(terminal_input_parser_test_fuzz
  PRIVATE src
)
target_link_libraries(terminal_input_parser_test_fuzz
  PRIVATE component
)
target_compile_options(terminal_input_parser_test_fuzz
  PRIVATE -fsanitize=fuzzer,address
)
target_link_libraries(terminal_input_parser_test_fuzz
  PRIVATE -fsanitize=fuzzer,address
)
