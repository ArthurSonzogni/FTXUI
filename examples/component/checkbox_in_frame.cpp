#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <memory>  // for unique_ptr, make_unique, __shared_ptr_access
#include <string>  // for operator+, wstring
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Checkbox, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, size, vbox, border, frame, Elements, HEIGHT, LESS_THAN
#include "ftxui/screen/string.hpp"  // for to_wstring

using namespace ftxui;

struct CheckboxAndState {
  std::wstring label;
  bool state;
  Component component;
};

std::unique_ptr<CheckboxAndState> MakeCheckbox(std::wstring label) {
  auto out = std::make_unique<CheckboxAndState>();
  out->label = label;
  out->state = false;
  out->component = Checkbox(&out->label, &out->state);
  return out;
}

class MyComponent : public ComponentBase {
 public:
  MyComponent() {
    Add(container);
    checkbox.resize(30);
    for (int i = 0; i < checkbox.size(); ++i) {
      checkbox[i] = MakeCheckbox(L"CheckBox " + to_wstring(i));
      container->Add(checkbox[i]->component);
    }
  }

  // clang-format off
  Element Render() override {
    Elements content;
    return vbox(container->Render())
      | frame
      | size(HEIGHT, LESS_THAN, 10)
      | border;
  }

 private:
  std::vector<std::unique_ptr<CheckboxAndState>> checkbox;
  Component container = Container::Vertical();
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();
  auto my_component = Make<MyComponent>();
  screen.Loop(my_component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
