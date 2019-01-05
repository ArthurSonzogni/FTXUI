#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen_interactive.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/component_vertical.hpp"
#include "ftxui/util/string.hpp"

using namespace ftxui::component;
using namespace ftxui::dom;
using namespace ftxui;

class MyComponent : ComponentVertical {
 public:
  MyComponent(ftxui::component::Delegate* delegate)
      : ComponentVertical(delegate),
        input_1(delegate->NewChild()),
        input_2(delegate->NewChild()),
        input_3(delegate->NewChild()) {

    input_1.placeholder = L"input1";
    input_2.placeholder = L"input2";
    input_3.placeholder = L"input3";
    Focus(&input_1);
  }

  std::function<void()> on_enter = []() {};

 private:
  Input input_1;
  Input input_2;
  Input input_3;

  Element Render() override {
    return
      frame(
        vbox(
          hbox(text(L" input_1 : "), input_1.Render()),
          hbox(text(L" input_2 : "), input_2.Render()),
          hbox(text(L" input_3 : "), input_3.Render())
        )
      );
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ftxui::ScreenInteractive::TerminalOutput();
  MyComponent component(screen.delegate());
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop();
}
