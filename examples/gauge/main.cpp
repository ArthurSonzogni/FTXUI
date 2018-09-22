#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/core/screen.hpp"
#include "ftxui/core/dom/elements.hpp"

int main(int argc, const char *argv[])
{
  for(float percentage = 0; percentage <= 1.0; percentage+=0.001) {
    using namespace ftxui::dom;
    auto document =
        hbox(text(L"gauge = -"), flex(gauge(percentage)), text(L"-"));
    auto screen = ftxui::Screen(100, 1);
    Render(screen, document.get());
    std::cout << '\r' << screen.ToString() << std::flush;

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}
