# Add dependency

.nxxm/deps
~~~json
{
  "ArthurSonzogni/FTXUI": {}
}
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

# Build

~~~
nxxm . -t clang-cxx17
~~~
