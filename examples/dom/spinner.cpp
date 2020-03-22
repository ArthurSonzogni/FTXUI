#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/string.hpp"

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  using namespace std::chrono_literals;

  std::string reset_position;
  for(int index = 0; index < 200; ++index) {
    std::vector<Element> entries;
    for(int i = 0; i<22; ++i) {
      if (i != 0)
      entries.push_back(separator());
      // clang-format off
      entries.push_back(
        hbox(
          text(to_wstring(i)) | size(WIDTH, EQUAL, 2),
          separator(),
          spinner(i, index) | bold
        )
      );
      // clang-format on
    }
    auto document = hbox(vbox(std::move(entries)) | border, filler());
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString() << std::flush;
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.1s);
  }
  std::cout << std::endl;
}
