#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/component_horizontal.hpp"
#include "ftxui/component/component_vertical.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui::component;
using namespace ftxui::dom;

class MyComponent : ComponentVertical {
 public:
  MyComponent(Delegate* delegate)
      : ComponentVertical(delegate),
        toggle_1(delegate->NewChild()),
        toggle_2(delegate->NewChild()),
        toggle_3(delegate->NewChild()),
        toggle_4(delegate->NewChild()) {
    toggle_1.options = {L"On", L"Off"};
    toggle_2.options = {L"Enabled", L"Disabled"};
    toggle_3.options = {L"10€", L"0€"};
    toggle_4.options = {L"Nothing", L"One element", L"Several elements"};

    Focus(&toggle_1);
  }

  std::function<void()> on_enter = []() {};

 private:
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

  bool OnEvent(Event event) override {
    if (ComponentVertical::OnEvent(event))
      return true;

    if (event == Event::Return) {
      on_enter();
      return true;
    }

    return false;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
