# FTXUI
![Demo image](./examples/component/homescreen.gif)
Functional Terminal (X) User interface

A simple  C++ library for terminal based user interface.

## Feature
 * Functional style.
 * Simple and elegant syntax (in my opinion).
 * No dependencies.

## Other features
 * vim navigation friendly (h,j,k,l)

## Example:
~~~cpp
  vbox(
    hbox(
      text(L"left") | border,
      text(L"middle") | border | flex,
      text(L"right") | border
    ),
    gauge(0.5) | border
  )
~~~

~~~bash
┌────┐┌───────────────────────────────────────────────────────────────┐┌─────┐
│left││middle                                                         ││right│
└────┘└───────────────────────────────────────────────────────────────┘└─────┘
┌────────────────────────────────────────────────────────────────────────────┐
│██████████████████████████████████████                                      │
└────────────────────────────────────────────────────────────────────────────┘
~~~

## Tutorial
See [Tutorial](./tutorial.md)

## Build using CMake
~~~bash
mkdir build && cd build
cmake ..
make
sudo make install
~~~

## Use library using CMake

CMakeLists.txt
~~~cmake
cmake_minimum_required(VERSION 3.0)

find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main PUBLIC ftxui::dom)
~~~

main.cpp
~~~cpp
#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  auto document =
    hbox(
      text(L"left") | bold,
      text(L"middle") | flex,
      text(L"right")
    ),
  auto screen = Screen::TerminalOutput(document);
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
~~~
