#include <memory>   // for allocator_traits<>...
#include <string>   // for operator+, wstring
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/component/checkbox.hpp"            // for CheckBox
#include "ftxui/component/component.hpp"           // for Component
#include "ftxui/component/container.hpp"           // for Container
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                  // for Element, operator|
#include "ftxui/screen/box.hpp"                    // for ftxui
#include "ftxui/screen/string.hpp"                 // for to_wstring

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
