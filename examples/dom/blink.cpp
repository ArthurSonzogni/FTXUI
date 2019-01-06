#include "ftxui/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  using namespace ftxui::dom;
  auto document =
    hbox(
      text(L"This text is "),
      text(L"blink") | blink,
      text(L". Do you like it?")
    );
  auto screen = ftxui::Screen::TerminalOutput(document);
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
