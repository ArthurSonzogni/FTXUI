cmake_minimum_required(VERSION 3.15)
project(test_package CXX)

find_package(ftxui CONFIG REQUIRED)

add_executable(example src/example.cpp)
target_compile_features(example PRIVATE cxx_std_17)
target_link_libraries(example PRIVATE ftxui::component)
