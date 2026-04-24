#include <typeinfo>
#include <initializer_list>
#include <array>                                  // for array
#include <cmath>                                  // for sin
#include <memory>                   // for shared_ptr, __shared_ptr_access
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

int main() {
  auto screen = App::TerminalOutput();
  std::array<int, 30> values;
  for (size_t i = 0; i < values.size(); ++i) {
    values[i] = 50 + 20 * std::sin(i * 0.3);
  }

  auto layout_horizontal = Container::Horizontal({});
  for (auto& value : values) {
    // In C++17:
    SliderOption<int> option;
    option.value = &value;
    option.max = 100;
    option.increment = 5;
    option.direction = Direction::Up;
    layout_horizontal->Add(Slider<int>(option));

    /* In C++20:
    layout_horizontal->Add(Slider<int>({
        .value = &values[i],
        .max = 100,
        .increment = 5,
        .direction = Direction::Up,
    }));
    */
  }

  layout_horizontal |= size(HEIGHT, GREATER_THAN, 20);

  screen.Loop(layout_horizontal);
}
