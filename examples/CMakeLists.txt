cmake_minimum_required(VERSION 3.0)

function(example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PUBLIC ftxui)
endfunction(example)

add_subdirectory(component)
add_subdirectory(dom)
example(print_key_press)
