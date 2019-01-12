#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::FixedSize(30, 3);
  Menu menu(screen.delegate());
  menu.entries = {L"entry 1", L"entry 2", L"entry 3"};
  menu.selected = 0;
  menu.on_enter = screen.ExitLoopClosure();

  screen.Loop();
}
