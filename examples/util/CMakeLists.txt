function(example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PUBLIC component)
  set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)
endfunction(example)

example(print_key_press)
