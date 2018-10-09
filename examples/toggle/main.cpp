#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/component/component_horizontal.hpp"
#include "ftxui/component/component_vertical.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui::component;
using namespace ftxui::dom;

class MyComponent : ComponentVertical {
 public:
  MyComponent(ftxui::component::Delegate* delegate)
      : ComponentVertical(delegate),
        toggle_1(delegate->NewChild()),
        toggle_2(delegate->NewChild()),
        toggle_3(delegate->NewChild()) {
    toggle_1.on = L"On";
    toggle_1.off = L"Off";

    toggle_2.on = L"Enabled";
    toggle_2.off = L"Disabled";

    toggle_3.on = L"10€";
    toggle_3.off = L"0€";

    Focus(&toggle_1);
  }

  std::function<void()> on_enter = []() {};

 private:
  Toggle toggle_1;
  Toggle toggle_2;
  Toggle toggle_3;

  Element Render() override {
    return
      vbox(
        text(L"Choose your options:"),
        text(L""),
        hbox(text(L" * Poweroff on startup      : "), toggle_1.Render()),
        hbox(text(L" * Out of process           : "), toggle_2.Render()),
        hbox(text(L" * Price of the information : "), toggle_3.Render())
      );
  }

  bool Event(int key) override {
    if (ComponentVertical::Event(key))
      return true;

    if (key == 10) {
      on_enter();
      return true;
    }

    return false;
  }
};

int main(int argc, const char* argv[]) {
  ftxui::ScreenInteractive screen(50,5);
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
