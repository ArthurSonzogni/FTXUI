#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  auto document =
    hbox(
      window(text(L" main frame ") | hcenter,
        vbox(
          text(L"Line 1"),
          text(L"Line 2"),
          text(L"Line 3"),
          vbox(
            text(L"Line 4"),
            text(L"Line 5"),
            text(L"Line 6")
          ) | border,
          hbox(
            window(text(L"frame 2"),
              vbox(
                text(L"Line 4"),
                text(L"Line 5"),
                text(L"Line 6")
              )
            ),
            window(text(L"frame 3"),
              vbox(
                text(L"Line 7"),
                text(L"Line 8"),
                text(L"Line 9")
              )
            )
          ),
          text(L"footer footer footer footer footer")
        )
      ),
      filler()
    );
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document.get());
  std::cout << screen.ToString() << std::endl;
}
