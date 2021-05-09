#include <functional>  // for function
#include <memory>      // for allocator, __shared_ptr_access
#include <string>      // for wstring, basic_string
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Toggle, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/event.hpp"           // for Event, Event::Return
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, vbox, Element

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::vector<std::wstring> toggle_1_entries_ = {
      L"On",
      L"Off",
  };
  std::vector<std::wstring> toggle_2_entries_ = {
      L"Enabled",
      L"Disabled",
  };
  std::vector<std::wstring> toggle_3_entries_ = {
      L"10€",
      L"0€",
  };
  std::vector<std::wstring> toggle_4_entries_ = {
      L"Nothing",
      L"One element",
      L"Several elements",
  };

  int toggle_1_selected_ = 0;
  int toggle_2_selected_ = 0;
  int toggle_3_selected_ = 0;
  int toggle_4_selected_ = 0;
  Component toggle_1_ = Toggle(&toggle_1_entries_, &toggle_1_selected_);
  Component toggle_2_ = Toggle(&toggle_2_entries_, &toggle_2_selected_);
  Component toggle_3_ = Toggle(&toggle_3_entries_, &toggle_3_selected_);
  Component toggle_4_ = Toggle(&toggle_4_entries_, &toggle_4_selected_);

  std::function<void()> exit_;

  Element Render() override {
    return vbox({
        text(L"Choose your options:"),
        text(L""),
        hbox(text(L" * Poweroff on startup      : "), toggle_1_->Render()),
        hbox(text(L" * Out of process           : "), toggle_2_->Render()),
        hbox(text(L" * Price of the information : "), toggle_3_->Render()),
        hbox(text(L" * Number of elements       : "), toggle_4_->Render()),
    });
  }

  bool OnEvent(Event event) override {
    if (event == Event::Return) {
      exit_();
      return true;
    }
    return ComponentBase::OnEvent(event);
  }

 public:
  MyComponent(std::function<void()> exit) : exit_(exit) {
    Add(Container::Vertical({
        toggle_1_,
        toggle_2_,
        toggle_3_,
        toggle_4_,
    }));
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(Make<MyComponent>(screen.ExitLoopClosure()));
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
