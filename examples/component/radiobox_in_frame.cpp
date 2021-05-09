#include <string>  // for wstring, operator+
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Make, Radiobox
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, size, border, frame, HEIGHT, LESS_THAN
#include "ftxui/screen/string.hpp"  // for to_wstring

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::vector<std::wstring> entries_;
  int selected_ = 0;

 public:
  MyComponent() {
    for (int i = 0; i < 30; ++i)
      entries_.push_back(L"RadioBox " + to_wstring(i));
    Add(Radiobox(&entries_, &selected_));
  }

  Element Render() override {
    return ComponentBase::Render() | frame | size(HEIGHT, LESS_THAN, 10) |
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
