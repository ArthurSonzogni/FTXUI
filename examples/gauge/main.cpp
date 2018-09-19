#include "ftxui/core/screen.hpp"
#include "ftxui/core/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  auto document =
    hbox(
      flex(vbox(
        gauge(0.1),
        gauge(0.2),
        gauge(0.3)
      )),
      flex(vbox(
        gauge(0.1),
        gauge(0.8),
        gauge(0.3)
      ))
    );
  //auto screen = ftxui::Screen::WholeTerminal();
  auto screen = ftxui::Screen::TerminalOutput(document);
  Render(screen, document.get());

  std::cout << screen.ToString();

  getchar();

  return 0;
}
