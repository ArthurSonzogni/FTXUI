cmake_minimum_required(VERSION 3.0)

add_subdirectory(component)
add_subdirectory(dom)

add_executable(print_key_press print_key_press.cpp)
target_link_libraries(print_key_press PUBLIC component)
