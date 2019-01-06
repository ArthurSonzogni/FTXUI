#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  using namespace ftxui::screen;
  auto document =
    dbox(
      frame(
        vbox(
          text(L"line_1"),
          text(L"line_2"),
          text(L"line_3"),
          text(L"line_4"),
          text(L"line_5")
        )
      ),
      center(
        frame(
          text(L"overlay")
        )
      )
    );
  auto screen = Screen::TerminalOutput(document);
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
