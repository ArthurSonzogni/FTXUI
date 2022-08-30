#include <array>                               // for array
#include <cmath>                               // for sin
#include <ftxui/component/component_base.hpp>  // for ComponentBase
#include <ftxui/dom/elements.hpp>  // for size, GaugeDirection, GaugeDirection::Up, GREATER_THAN, HEIGHT
#include <memory>  // for shared_ptr, __shared_ptr_access

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Horizontal, Slider, operator|=
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

using namespace ftxui;

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  std::array<int, 30> values;
  for (int i = 0; i < values.size(); ++i) {
    values[i] = 50 + 20 * std::sin(i * 0.3);
  }

  auto layout_horizontal = Container::Horizontal({});
  for (int i = 0; i < values.size(); ++i) {
    layout_horizontal->Add(Slider<int>({
        .value = &values[i],
        .max = 100,
        .increment = 5,
        .direction = GaugeDirection::Up,
    }));
  }

  layout_horizontal |= size(HEIGHT, GREATER_THAN, 20);

  screen.Loop(layout_horizontal);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
