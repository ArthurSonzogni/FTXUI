#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
  RadioBox radiobox;

 public:
  MyComponent() {
    for (int i = 0; i < 30; ++i) {
      radiobox.entries.push_back(L"RadioBox " + to_wstring(i));
    }
    Add(&radiobox);
  }

  Element Render() override {
    return radiobox.Render() | frame | size(HEIGHT, LESS_THAN, 10) | border;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();
  MyComponent component;
  screen.Loop(&component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
