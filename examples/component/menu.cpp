#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen_interactive.hpp"
#include "ftxui/component/menu.hpp"

int main(int argc, const char *argv[])
{
  ftxui::ScreenInteractive screen(30,3);
  ftxui::component::Menu menu(screen.delegate());
  menu.entries = {
    L"entry 1",
    L"entry 2",
    L"entry 3"
  };
  menu.selected = 0;
  menu.on_enter = screen.ExitLoopClosure();

  screen.Loop();
}
