set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

function(fuzz name)
  add_executable(${name}
    src/ftxui/component/${name}.cpp
  )
  target_include_directories(${name}
    PRIVATE src
  )
  target_link_libraries(${name}
    PRIVATE component
  )
  target_compile_options(${name}
    PRIVATE -fsanitize=fuzzer,address
  )
  target_link_libraries(${name}
    PRIVATE -fsanitize=fuzzer,address
  )
endfunction(fuzz)

fuzz(terminal_input_parser_test_fuzzer)
fuzz(component_fuzzer)
