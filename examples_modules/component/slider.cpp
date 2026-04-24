#include <typeinfo>
#include <initializer_list>
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

using namespace ftxui;

int main() {
  auto screen = App::TerminalOutput();
  int value = 50;
  auto slider = Slider("Value:", &value, 0, 100, 1);
  screen.Loop(slider);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
