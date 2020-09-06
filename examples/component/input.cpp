#include "ftxui/component/input.hpp"

#include <iostream>

#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container);
    container.Add(&input_1);
    container.Add(&input_2);
    container.Add(&input_3);

    input_1.placeholder = L"input1";
    input_2.placeholder = L"input2";
    input_3.placeholder = L"input3";
  }

  std::function<void()> on_enter = []() {};

 private:
  Container container = Container::Vertical();
  Input input_1;
  Input input_2;
  Input input_3;

  Element Render() override {
    return border(vbox({
        hbox({text(L" input_1 : "), input_1.Render()}),
        hbox({text(L" input_2 : "), input_2.Render()}),
        hbox({text(L" input_3 : "), input_3.Render()}),
    }));
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
