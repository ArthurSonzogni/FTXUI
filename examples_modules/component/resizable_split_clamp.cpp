#include <typeinfo>
#include <initializer_list>
#include <memory>  // for shared_ptr, allocator, __shared_ptr_access
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

int main() {
  auto screen = App::Fullscreen();

  // State:
  int size = 40;
  int size_min = 10;
  int size_max = 80;

  // Renderers:
  auto split = ResizableSplit({
      .main = Renderer([] { return text("Left") | center; }),
      .back = Renderer([] { return text("Right") | center; }),
      .direction = Direction::Left,
      .main_size = &size,
      .min = &size_min,
      .max = &size_max,
  });

  auto renderer = Renderer(split, [&] {
    return window(text("Drag the separator with the mouse"),
                  vbox({
                      text("Min:  " + std::to_string(size_min)),
                      text("Max:  " + std::to_string(size_max)),
                      text("Size: " + std::to_string(size)),
                      separator(),
                      split->Render() | flex,
                  }));
  });

  screen.Loop(renderer);
}
