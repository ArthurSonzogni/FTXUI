#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Slider
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::TerminalOutput();
  int value = 50;
  auto slider = Slider("Value:", &value, 0, 100, 1);
  screen.Loop(slider);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
