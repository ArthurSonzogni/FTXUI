#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include <iostream>

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  auto document =
    hbox(
      text(L"This text is "),
      text(L"dim") | dim,
      text(L". Do you like it?")
    );
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document.get());

  std::cout << screen.ToString();

  return 0;
}
