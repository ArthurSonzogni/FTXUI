#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <array>
#include <iostream>
#include <string>

using namespace ftxui;
using namespace std;

enum class ConnectionMethod { ByName, ByAddress };

void promptForConnectionMethod();

void promptForServerName();
void promptForServerAddress();
void promptForServerManager();

int main() {
  promptForConnectionMethod();

  return 0;
}

void promptForConnectionMethod() {
  auto screen = ScreenInteractive::TerminalOutput();
  std::vector<std::string> connect_choice{
      "Connect by name...",
      "Connect by address and port...",
      "Exit",
  };
  int selected = 0;
  MenuOption option;
  option.on_enter = [&] {
    if (selected == 0) {
      promptForServerManager();
    } else if (selected == 1) {
      promptForServerAddress();
    } else if (selected == 2) {
      screen.Exit();
    }
  };

  auto connect_menu = Menu(&connect_choice, &selected, option);

  auto renderer = Renderer(connect_menu, [&] {
    return vbox({
               text("Welcome, my client!") | color(Color::Red3Bis) | bold |
                   center,
               text(""),
               text("Selected = " + std::to_string(selected)) |
                   color(Color::LightGreenBis) | bold | center,

               text(""),
               text("Welcome to my first working multiplayer game, Medium "
                    "Boxes.") |
                   color(Color::LightSkyBlue1),
               text(""),
               text("Now, choose how you'd prefer to connect to a server!") |
                   color(Color::LightCyan3) | center,
               text(""),
               connect_menu->Render() | border,
           }) |
           center;
  });

  screen.Loop(renderer);
}

void promptForServerName() {}

void promptForServerAddress() {
  auto screen = ScreenInteractive::TerminalOutput();
}

void promptForServerManager() {
  auto screen = ScreenInteractive::TerminalOutput();
  auto renderer = Renderer([&] {
    return vbox({
               text("Now, please enter the server manager's address and "
                    "port, so that you'll be able to see all the available "
                    "public servers!") |
                   color(Color::LightGreenBis),
               gauge(0),
           }) |
           center;
  });

  screen.Loop(renderer);
}
