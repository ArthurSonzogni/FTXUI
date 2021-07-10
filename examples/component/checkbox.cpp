#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Checkbox, Vertical
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

using namespace ftxui;

int main(int argc, const char* argv[]) {
  bool build_examples_state = false;
  bool build_tests_state = false;
  bool use_webassembly_state = true;

  auto component = Container::Vertical({
      Checkbox("Build examples", &build_examples_state),
      Checkbox("Build tests", &build_tests_state),
      Checkbox("Use WebAssembly", &use_webassembly_state),
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(component);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
