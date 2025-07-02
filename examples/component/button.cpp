// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

int main() {
  auto screen = ftxui::ScreenInteractive::Fullscreen();
  auto testComponent =
      ftxui::Renderer([]() { return ftxui::text("test Component"); });
  screen.Loop(testComponent);
  return 0;
}
