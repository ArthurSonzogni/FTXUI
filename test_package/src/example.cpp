#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main() {
  using namespace ftxui;
  auto document = hbox({
      text(" Hello "),
      text("FTXUI ") | bold | color(Color::Red),
      text(" world! "),
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  std::cout << screen.ToString() << std::endl;
  return 0;
}
