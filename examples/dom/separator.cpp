#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox(
      text(L"left-column"),
      separator(),
      vbox(
        center(text(L"right-top")) | flex,
        separator(),
        center(text(L"bottom-bottom"))
      ) | flex,
      separator(),
      text(L"right-column")
    ) | border;
  // clang-format on
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document.get());

  std::cout << screen.ToString() << std::endl;

  return 0;
}
