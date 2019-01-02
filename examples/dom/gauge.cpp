#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen.hpp"
#include "ftxui/dom/elements.hpp"

int main(int argc, const char *argv[])
{
  for(float percentage = 0; percentage <= 1.0; percentage+=0.002) {
    std::wstring data_downloaded =
        std::to_wstring(int(percentage * 5000)) + L"/5000";
    using namespace ftxui::dom;
    auto document =
        hbox(
          text(L"downloading:"),
          flex(gauge(percentage)),
          text(L" " + data_downloaded)
        );
    auto screen = ftxui::Screen(100, 1);
    Render(screen, document.get());
    std::cout << '\r' << screen.ToString() << std::flush;

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}
