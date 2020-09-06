#include "ftxui/component/toggle.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container_);
    container_.Add(&toggle_1_);
    container_.Add(&toggle_2_);
    container_.Add(&toggle_3_);
    container_.Add(&toggle_4_);

    toggle_1_.entries = {L"On", L"Off"};
    toggle_2_.entries = {L"Enabled", L"Disabled"};
    toggle_3_.entries = {L"10€", L"0€"};
    toggle_4_.entries = {L"Nothing", L"One element", L"Several elements"};
  }

  std::function<void()> on_enter = []() {};

 private:
  Container container_ = Container::Vertical();
  Toggle toggle_1_;
  Toggle toggle_2_;
  Toggle toggle_3_;
  Toggle toggle_4_;

  Element Render() override {
    return vbox({
        text(L"Choose your options:"),
        text(L""),
        hbox(text(L" * Poweroff on startup      : "), toggle_1_.Render()),
        hbox(text(L" * Out of process           : "), toggle_2_.Render()),
        hbox(text(L" * Price of the information : "), toggle_3_.Render()),
        hbox(text(L" * Number of elements       : "), toggle_4_.Render()),
    });
  }

  bool OnEvent(Event event) override {
    if (event == Event::Return) {
      on_enter();
      return true;
    }
    return Component::OnEvent(event);
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
