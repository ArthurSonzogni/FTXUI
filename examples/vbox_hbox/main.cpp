#include "ftxui/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  auto document =
    vbox(
      hbox(
        text(L"north-west"),
        flex(),
        text(L"north-east")
      ),
      flex(),
      hbox(
        hbox(
          flex(),
          text(L"center"),
          flex()
        )
      ),
      flex(),
      hbox(
        text(L"south-west"),
        flex(),
        text(L"south-east")
      )
    );
  auto screen = ftxui::Screen::TerminalFullscreen();
  Render(screen, document.get());

  std::cout << screen.ToString();

  getchar();

  return 0;
}
