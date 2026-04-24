#include <typeinfo>
#include <initializer_list>
#include <memory>                         // for allocator, shared_ptr
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;
  auto document = text("gradient") | center;

  document |= bgcolor(LinearGradient()
                          .Angle(45)
                          .Stop(Color::DeepPink1)
                          .Stop(Color::DeepSkyBlue1));
  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
  Render(screen, document);
  screen.Print();

  return 0;
}
