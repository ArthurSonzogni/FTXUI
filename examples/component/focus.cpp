// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for operator+, char_traits, to_string, string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Slider, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Elements, Element, operator|, separator, text, focusPositionRelative, size, border, flex, frame, bgcolor, gridbox, vbox, EQUAL, center, HEIGHT, WIDTH
#include "ftxui/screen/color.hpp"  // for Color

using namespace ftxui;

Element make_box(int x, int y) {
  std::string title = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
  return text(title) | center | size(WIDTH, EQUAL, 18) |
         size(HEIGHT, EQUAL, 9) | border |
         bgcolor(Color::HSV(x * 255 / 15, 255, y * 255 / 15));
};

Element make_grid() {
  std::vector<Elements> rows;
  for (int i = 0; i < 15; i++) {
    std::vector<Element> cols;
    for (int j = 0; j < 15; j++) {
      cols.push_back(make_box(i, j));
    }
    rows.push_back(cols);
  }

  return gridbox(rows);
};

int main() {
  float focus_x = 0.5f;
  float focus_y = 0.5f;

  auto slider_x = Slider("x", &focus_x, 0.f, 1.f, 0.01f);
  auto slider_y = Slider("y", &focus_y, 0.f, 1.f, 0.01f);

  auto renderer = Renderer(
      Container::Vertical({
          slider_x,
          slider_y,
      }),
      [&] {
        auto title = "focusPositionRelative(" +        //
                     std::to_string(focus_x) + ", " +  //
                     std::to_string(focus_y) + ")";    //
        return vbox({
                   text(title),
                   separator(),
                   slider_x->Render(),
                   slider_y->Render(),
                   separator(),
                   make_grid() | focusPositionRelative(focus_x, focus_y) |
                       frame | flex,
               }) |
               border;
      });

  auto screen = ScreenInteractive::Fullscreen();
  screen.Loop(renderer);

  return 0;
}
