function(example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PUBLIC component)
endfunction(example)

example(print_key_press)
