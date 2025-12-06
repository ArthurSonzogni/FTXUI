// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

int main() {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();
  auto component = Button("Quit", screen.ExitLoopClosure());
  screen.Loop(component);
  return 0;
}

