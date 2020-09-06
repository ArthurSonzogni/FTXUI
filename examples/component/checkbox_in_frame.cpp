#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container);
    checkbox.resize(30);
    for (int i = 0; i < checkbox.size(); ++i) {
      checkbox[i].label = (L"CheckBox " + to_wstring(i));
      container.Add(&checkbox[i]);
    }
  }

  // clang-format off
  Element Render() override {
    Elements content;
    for (auto& it : checkbox) {
      content.push_back(it.Render());
    }
    return vbox(std::move(content))
      | frame
      | size(HEIGHT, LESS_THAN, 10)
      | border;
  }

 private:
  std::vector<CheckBox> checkbox;
  Container container = Container::Vertical();
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
