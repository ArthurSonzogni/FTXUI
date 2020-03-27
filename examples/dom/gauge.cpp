#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  using namespace std::chrono_literals;

  std::string reset_position;
  for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.002f) {
    std::wstring data_downloaded =
        std::to_wstring(int(percentage * 5000)) + L"/5000";
    // clang-format off
    auto document =
        hbox(
          text(L"downloading:"),
          gauge(percentage) | flex,
          text(L" " + data_downloaded)
        );
    // clang-format on
    auto screen = Screen(100, 1);
    Render(screen, document.get());
    std::cout << reset_position << screen.ToString() << std::flush;
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}
