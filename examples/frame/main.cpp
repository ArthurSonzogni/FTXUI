#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/core/screen.hpp"
#include "ftxui/core/dom/elements.hpp"

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  auto document =
    hbox(
      frame(
        vbox(
          text(L"Line 1"),
          text(L"Line 2"),
          text(L"Line 3"),
          frame(
            vbox(
              text(L"Line 4"),
              text(L"Line 5"),
              text(L"Line 6")
            )
          ),
          text(L"Line 7"),
          text(L"Line 8"),
          text(L"Line 9")
        )
      ),
      flex()
    );
  auto screen = ftxui::Screen::TerminalOutput(document);
  Render(screen, document.get());
  std::cout << screen.ToString() << std::endl;
}
