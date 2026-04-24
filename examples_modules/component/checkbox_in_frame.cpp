#include <typeinfo>
#include <initializer_list>
#include <array>   // for array
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

int main() {
  std::array<bool, 30> states;

  auto container = Container::Vertical({});
  for (int i = 0; i < 30; ++i) {
    states[i] = false;
    container->Add(Checkbox("Checkbox" + std::to_string(i), &states[i]));
  }

  auto renderer = Renderer(container, [&] {
    return container->Render() | vscroll_indicator | frame |
           size(HEIGHT, LESS_THAN, 10) | border;
  });

  auto screen = App::FitComponent();
  screen.Loop(renderer);

  return 0;
}
