#include "ftxui/core/screen.hpp"
#include "ftxui/core/dom/elements.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui::dom;
  auto root =
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
  auto screen = ftxui::Screen::WholeTerminal();
  Render(screen, root.get());

  std::cout << screen.ToString();

  getchar();

  return 0;
}
