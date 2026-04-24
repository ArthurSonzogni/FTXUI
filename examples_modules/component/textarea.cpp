#include <typeinfo>
#include <initializer_list>
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;

  std::string content_1;
  std::string content_2;
  auto textarea_1 = Input(&content_1);
  auto textarea_2 = Input(&content_2);
  int size = 50;
  auto layout = ResizableSplitLeft(textarea_1, textarea_2, &size);

  auto component = Renderer(layout, [&] {
    return vbox({
               text("Input:"),
               separator(),
               layout->Render() | flex,
           }) |
           border;
  });

  auto screen = App::Fullscreen();
  screen.Loop(component);
}
