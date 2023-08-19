// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/component_base.hpp>  // for ComponentBase, Component
#include <ftxui/dom/elements.hpp>  // for operator|, Element, flex, bgcolor, text, vbox, center
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient
#include <ftxui/screen/color.hpp>         // for Color, Color::Blue, Color::Red
#include <memory>                         // for __shared_ptr_access, shared_ptr
#include <string>  // for allocator, operator+, char_traits, string, to_string

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Slider, Renderer, Vertical
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main() {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen();

  int angle = 180.f;
  float start = 0.f;
  float end = 1.f;

  std::string slider_angle_text;
  std::string slider_start_text;
  std::string slider_end_text;

  auto slider_angle = Slider(&slider_angle_text, &angle, 0, 360);
  auto slider_start = Slider(&slider_start_text, &start, 0.f, 1.f, 0.05f);
  auto slider_end = Slider(&slider_end_text, &end, 0.f, 1.f, 0.05f);

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
                      bgcolor(LinearGradient()
                                  .Angle(angle)
                                  .Stop(Color::Blue, start)
                                  .Stop(Color::Red, end));
    return vbox({
               background | flex,
               layout->Render(),
           }) |
           flex;
  });

  screen.Loop(renderer);
}
