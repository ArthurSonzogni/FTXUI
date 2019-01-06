#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/string.hpp"

int main(int argc, const char *argv[])
{
  using namespace ftxui;
  using namespace ftxui::dom;
  using namespace ftxui::screen;
  using namespace std::chrono_literals;

  std::string reset_position;
  for(int index = 0; index < 200; ++index) {
    std::vector<Element> entries;
    for(int i = 0; i<22; ++i) {
      if (i != 0)
      entries.push_back(separator());
      entries.push_back(
        hbox(
          text(to_wstring(i)) | size(5,1),
          spinner(i, index) | bold
        )
      );
    }
    auto document = hbox(vbox(std::move(entries)) | frame, filler());
    auto screen = Screen::TerminalOutput(document);
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString() << std::flush;
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.1s);
  }
  std::cout << std::endl;
}
