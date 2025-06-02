// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/dom/linear_gradient.hpp>  // for LinearGradient
#include <ftxui/screen/screen.hpp>        // for Full, Screen
#include <memory>                         // for allocator

#include "ftxui/dom/elements.hpp"  // for text, bgcolor, color, vbox, filler, Fit, hbox
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for Color, operator""_rgb, Color::Black, Color::Blue, Color::BlueLight, Color::Cyan, Color::CyanLight, Color::DeepSkyBlue4, Color::Default, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::RedLight, Color::SkyBlue1, Color::White, Color::Yellow, Color::YellowLight, ftxui

int main() {
  using namespace ftxui;
  auto document = hbox({
      vbox({
          color(Color::Default, text("Default")),
          color(Color::Black, text("Black")),
          color(Color::GrayDark, text("GrayDark")),
          color(Color::GrayLight, text("GrayLight")),
          color(Color::White, text("White")),
          color(Color::Blue, text("Blue")),
          color(Color::BlueLight, text("BlueLight")),
          color(Color::Cyan, text("Cyan")),
          color(Color::CyanLight, text("CyanLight")),
          color(Color::Green, text("Green")),
          color(Color::GreenLight, text("GreenLight")),
          color(Color::Magenta, text("Magenta")),
          color(Color::MagentaLight, text("MagentaLight")),
          color(Color::Red, text("Red")),
          color(Color::RedLight, text("RedLight")),
          color(Color::Yellow, text("Yellow")),
          color(Color::YellowLight, text("YellowLight")),
          color(0x66ff66_rgb, text("Phosphor")),
          color(LinearGradient(Color::SkyBlue1, Color::DeepSkyBlue4),
                text("Skyblue to DeepSkyBlue")),
      }),
      vbox({
          bgcolor(Color::Default, text("Default")),
          bgcolor(Color::Black, text("Black")),
          bgcolor(Color::GrayDark, text("GrayDark")),
          bgcolor(Color::GrayLight, text("GrayLight")),
          bgcolor(Color::White, text("White")),
          bgcolor(Color::Blue, text("Blue")),
          bgcolor(Color::BlueLight, text("BlueLight")),
          bgcolor(Color::Cyan, text("Cyan")),
          bgcolor(Color::CyanLight, text("CyanLight")),
          bgcolor(Color::Green, text("Green")),
          bgcolor(Color::GreenLight, text("GreenLight")),
          bgcolor(Color::Magenta, text("Magenta")),
          bgcolor(Color::MagentaLight, text("MagentaLight")),
          bgcolor(Color::Red, text("Red")),
          bgcolor(Color::RedLight, text("RedLight")),
          bgcolor(Color::Yellow, text("Yellow")),
          bgcolor(Color::YellowLight, text("YellowLight")),
          bgcolor(0x66ff66_rgb, text("Phosphor")),
          bgcolor(LinearGradient(Color::SkyBlue1, Color::DeepSkyBlue4),
                  text("Skyblue to DeepSkyBlue")),
      }),
      filler(),
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}
