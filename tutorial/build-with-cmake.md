# Build ftxui and install

~~~bash
mkdir build && cd build
cmake ..
make
sudo make install
~~~

# Example project

main.cpp
~~~cpp
#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[]) {
  using namespace ftxui;
  auto document =
    hbox({
      text(L"left") | bold | border,
      text(L"middle") | flex | border,
      text(L"right") | border,
      });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}
~~~

# Add dependency

CMakeLists.txt
~~~cmake
cmake_minimum_required(VERSION 3.0)

find_package(ftxui REQUIRED)
add_executable(main main.cpp)
target_link_libraries(main PUBLIC ftxui::dom)
~~~

# Build
~~~
mkdir build && cd build
cmake ..
make
./main
~~~
