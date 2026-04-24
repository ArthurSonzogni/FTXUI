// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <array>                                  // for array
#include <cmath>                                  // for sin
#include <ftxui/component/component_base.hpp>     // for ComponentBase
#include <ftxui/component/component_options.hpp>  // for SliderOption
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Up
#include <ftxui/dom/elements.hpp>   // for size, GREATER_THAN, HEIGHT
#include <ftxui/util/ref.hpp>       // for ConstRef, Ref
#include <memory>                   // for shared_ptr, __shared_ptr_access

#include "ftxui/component/app.hpp"             // for App
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Horizontal, Slider, operator|=

using namespace ftxui;

int main() {
  auto screen = App::TerminalOutput();
  std::array<int, 30> values;
  for (size_t i = 0; i < values.size(); ++i) {
    values[i] = static_cast<int>(50.0 + 20.0 * std::sin(i * 0.3));
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
