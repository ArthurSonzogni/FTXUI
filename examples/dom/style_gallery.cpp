#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox(
      text(L"normal")                           , text(L" ") ,
      text(L"bold")      | bold                 , text(L" ") ,
      text(L"dim")       | dim                  , text(L" ") ,
      text(L"inverted")  | inverted             , text(L" ") ,
      text(L"underlined")| underlined           , text(L" ") ,
      text(L"blink")     | blink                , text(L" ") ,
      text(L"color")     | color(Color::Blue)   , text(L" ") ,
      text(L"bgcolor")   | bgcolor(Color::Blue)
    );
  // clang-format on
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
