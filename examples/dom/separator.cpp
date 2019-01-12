#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  auto document =
    hbox(
      text(L"left-column"),
      separator(),
      flex(vbox(
        center(text(L"right-column")) | flex,
        separator(),
        center(text(L"bottom-column"))
      ))
    );
  auto screen = Screen::TerminalFullscreen();
  Render(screen, document.get());

  std::cout << screen.ToString();
  getchar();

  return 0;
}
