#include "ftxui/core/screen.hpp"
#include "ftxui/core/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  auto document =
    hbox(
      text(L"left-column"),
      separator(),
      flex(vbox(
        flex(center(text(L"right-column"))),
        separator(),
        center(text(L"bottom-column"))
      ))
    );
  auto screen = ftxui::Screen::WholeTerminal();
  Render(screen, document.get());

  std::cout << screen.ToString();

  getchar();

  return 0;
}
