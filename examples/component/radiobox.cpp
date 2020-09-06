#include "ftxui/component/radiobox.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  RadioBox radiobox;
  radiobox.entries = {
      L"Use gcc",
      L"Use clang",
      L"Use emscripten",
      L"Use tcc",
  };
  screen.Loop(&radiobox);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
