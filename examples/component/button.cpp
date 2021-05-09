#include <string>  // for operator+, to_wstring, allocator, wstring

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Button, Make
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/container.hpp"           // for Container
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, Element, gauge, text, operator|, vbox, border

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::wstring label_add = L"Increase";
  std::wstring label_rm = L"Decrease";
  int value_ = 50;

 public:
  MyComponent() {
    Add(Container::Horizontal({
        Button(&label_rm, [&] { value_--; }),
        Button(&label_add, [&] { value_++; }),
    }));
  }

  Element Render() override {
    return vbox({
               text(L"Value = " + std::to_wstring(value_)),
               separator(),
               gauge(value_ * 0.01f),
               separator(),
               ComponentBase::Render(),
           }) |
           border;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(Make<MyComponent>());
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
