if (NOT FTXUI_BUILD_TESTS_FUZZER)
  return()
endif()

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

function(fuzz source)
  set(name "ftxui-${source}")
  add_executable(${name}
    src/ftxui/component/${source}.cpp
  )
  target_include_directories(${name} PRIVATE src)
  target_link_libraries(${name} PRIVATE component)
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer,address)
  target_link_libraries(${name} PRIVATE -fsanitize=fuzzer,address)
  set_target_properties(${name} PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
  )
endfunction(fuzz)

fuzz(terminal_input_parser_test_fuzzer)
fuzz(component_fuzzer)
