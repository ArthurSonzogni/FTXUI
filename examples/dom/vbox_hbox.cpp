#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    vbox(
      hbox(
        text(L"north-west"),
        filler(),
        text(L"north-east")
      ),
      filler(),
      hbox(
        hbox(
          filler(),
          text(L"center"),
          filler()
        )
      ),
      filler(),
      hbox(
        text(L"south-west"),
        filler(),
        text(L"south-east")
      )
    );
  // clang-format on
  auto screen = Screen::Create(Dimension::Full());
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
