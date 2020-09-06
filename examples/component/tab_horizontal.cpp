#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container_);
    container_.Add(&toggle_);

    toggle_.entries = {
        L"tab_1",
        L"tab_2",
        L"tab_3",
    };

    container_.Add(&tab_container_);

    radiobox_1_.entries = {L"Forest", L"Water", L"I don't know"};
    tab_container_.Add(&radiobox_1_);

    radiobox_2_.entries = {
        L"Hello",
        L"Hi",
        L"Hay",
    };
    tab_container_.Add(&radiobox_2_);

    radiobox_3_.entries = {
        L"Table",
        L"Nothing",
        L"Is",
        L"Empty",
    };
    tab_container_.Add(&radiobox_3_);
  }

  std::function<void()> on_enter = []() {};

  Element Render() {
    return vbox(toggle_.Render(), separator(), tab_container_.Render()) |
           border;
  }

 private:
  Toggle toggle_;
  Container container_ = Container::Vertical();
  Container tab_container_ = Container::Tab(&(toggle_.selected));
  RadioBox radiobox_1_;
  RadioBox radiobox_2_;
  RadioBox radiobox_3_;
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
