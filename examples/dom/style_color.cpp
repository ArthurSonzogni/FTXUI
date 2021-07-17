#include <ftxui/screen/screen.hpp>  // for Dimension, Screen
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/elements.hpp"    // for bgcolor, color, vbox, filler, hbox
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui
#include "ftxui/screen/color.hpp"  // for Color, Color::Black, Color::Blue, Color::BlueLight, Color::Cyan, Color::CyanLight, Color::Default, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::RedLight, Color::White, Color::Yellow, Color::YellowLight

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox(
      vbox(
        color(Color::Default, text(L"Default")),
        color(Color::Black, text(L"Black")),
        color(Color::GrayDark, text(L"GrayDark")),
        color(Color::GrayLight, text(L"GrayLight")),
        color(Color::White, text(L"White")),
        color(Color::Blue, text(L"Blue")),
        color(Color::BlueLight, text(L"BlueLight")),
        color(Color::Cyan, text(L"Cyan")),
        color(Color::CyanLight, text(L"CyanLight")),
        color(Color::Green, text(L"Green")),
        color(Color::GreenLight, text(L"GreenLight")),
        color(Color::Magenta, text(L"Magenta")),
        color(Color::MagentaLight, text(L"MagentaLight")),
        color(Color::Red, text(L"Red")),
        color(Color::RedLight, text(L"RedLight")),
        color(Color::Yellow, text(L"Yellow")),
        color(Color::YellowLight, text(L"YellowLight"))
      ),
      vbox(
        bgcolor(Color::Default, text(L"Default")),
        bgcolor(Color::Black, text(L"Black")),
        bgcolor(Color::GrayDark, text(L"GrayDark")),
        bgcolor(Color::GrayLight, text(L"GrayLight")),
        bgcolor(Color::White, text(L"White")),
        bgcolor(Color::Blue, text(L"Blue")),
        bgcolor(Color::BlueLight, text(L"BlueLight")),
        bgcolor(Color::Cyan, text(L"Cyan")),
        bgcolor(Color::CyanLight, text(L"CyanLight")),
        bgcolor(Color::Green, text(L"Green")),
        bgcolor(Color::GreenLight, text(L"GreenLight")),
        bgcolor(Color::Magenta, text(L"Magenta")),
        bgcolor(Color::MagentaLight, text(L"MagentaLight")),
        bgcolor(Color::Red, text(L"Red")),
        bgcolor(Color::RedLight, text(L"RedLight")),
        bgcolor(Color::Yellow, text(L"Yellow")),
        bgcolor(Color::YellowLight, text(L"YellowLight"))
      ),
      filler()
    );
  // clang-format on

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
