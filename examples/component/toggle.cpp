#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container);
    container.Add(&toggle_1);
    container.Add(&toggle_2);
    container.Add(&toggle_3);
    container.Add(&toggle_4);

    toggle_1.options = {L"On", L"Off"};
    toggle_2.options = {L"Enabled", L"Disabled"};
    toggle_3.options = {L"10€", L"0€"};
    toggle_4.options = {L"Nothing", L"One element", L"Several elements"};
  }

  std::function<void()> on_enter = []() {};

 private:
  Container container = Container::Vertical();
  Toggle toggle_1;
  Toggle toggle_2;
  Toggle toggle_3;
  Toggle toggle_4;

  Element Render() override {
    return
      vbox(
        text(L"Choose your options:"),
        text(L""),
        hbox(text(L" * Poweroff on startup      : "), toggle_1.Render()),
        hbox(text(L" * Out of process           : "), toggle_2.Render()),
        hbox(text(L" * Price of the information : "), toggle_3.Render()),
        hbox(text(L" * Number of elements       : "), toggle_4.Render())
      );
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}
