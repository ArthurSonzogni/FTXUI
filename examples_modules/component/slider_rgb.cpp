#include <typeinfo>
#include <initializer_list>
#include <memory>  // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for char_traits, operator+, to_string
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


using namespace ftxui;

Element ColorTile(int red, int green, int blue) {
  return text("") | size(WIDTH, GREATER_THAN, 14) |
         size(HEIGHT, GREATER_THAN, 7) | bgcolor(Color::RGB(red, green, blue));
}

Element ColorString(int red, int green, int blue) {
  return text("RGB = (" +                    //
              std::to_string(red) + "," +    //
              std::to_string(green) + "," +  //
              std::to_string(blue) + ")"     //
  );
}

int main() {
  int red = 128;
  int green = 25;
  int blue = 100;
  auto slider_red = Slider("Red  :", &red, 0, 255, 1);
  auto slider_green = Slider("Green:", &green, 0, 255, 1);
  auto slider_blue = Slider("Blue :", &blue, 0, 255, 1);

  auto container = Container::Vertical({
      slider_red,
      slider_green,
      slider_blue,
  });

  auto renderer = Renderer(container, [&] {
    return hbox({
               ColorTile(red, green, blue),
               separator(),
               vbox({
                   slider_red->Render(),
                   separator(),
                   slider_green->Render(),
                   separator(),
                   slider_blue->Render(),
                   separator(),
                   ColorString(red, green, blue),
               }) | xflex,
           }) |
           border | size(WIDTH, LESS_THAN, 80);
  });
  auto screen = App::TerminalOutput();
  screen.Loop(renderer);
}
