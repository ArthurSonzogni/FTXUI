#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for operator+, wstring, char_traits

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, border, vbox, Element

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::wstring first_name_;
  std::wstring last_name_;
  std::wstring first_name_placeholder_ = L"first_name";
  std::wstring last_name_placeholder_ = L"last_name";
  Component input_first_name_ = Input(&first_name_, &first_name_placeholder_);
  Component input_last_name_ = Input(&last_name_, &last_name_placeholder_);

  Element Render() override {
    return border(vbox({
        text(L"Hello " + first_name_ + L" " + last_name_),
        separator(),
        hbox({text(L" First name  : "), input_first_name_->Render()}),
        hbox({text(L" Last name   : "), input_last_name_->Render()}),
    }));
  }

 public:
  MyComponent() {
    Add(Container::Vertical({
        input_first_name_,
        input_last_name_,
    }));
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(Make<MyComponent>());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
