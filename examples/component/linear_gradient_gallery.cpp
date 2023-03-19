#include <functional>  // for function
#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <string>    // for string, basic_string, allocator
#include <vector>    // for vector

#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/component_options.hpp"   // for MenuOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen();

  int angle = 180.f;
  float start = 0.f;
  float end = 1.f;

  std::string slider_angle_text;
  std::string slider_start_text;
  std::string slider_end_text;

  auto slider_angle = Slider(&slider_angle_text, &angle, 0, 360);
  auto slider_start = Slider(&slider_start_text, &start, 0.f, 1.f);
  auto slider_end = Slider(&slider_end_text, &end, 0.f, 1.f);

  auto layout = Container::Vertical({
    slider_angle,
    slider_start,
    slider_end,
  });

  auto renderer = Renderer(layout, [&] {
    slider_angle_text = "angle = " + std::to_string(angle) + "°";
    slider_start_text = "start = " + std::to_string(int(start * 100)) + "%";
    slider_end_text = "end   = " + std::to_string(int(end * 100)) + "%";

    auto background = text("Gradient") | center |
                      bgcolor(LinearGradient(
                          angle, {{Color::Red, start}, {Color::Blue, end}}));
    return vbox({
               background | flex,
               layout->Render(),
           }) |
           flex;
  });

  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
