#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <iostream>

#include "ftxui/screen/string.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto terminal_info =
    vbox(
      text(L"Basic Color support : unknown"),
      text(L"256 Color support : unknown"),
      (Terminal::CanSupportTrueColors() ?
        text(L"TrueColor support : Yes"):
        text(L"TrueColor support : No"))
    );

  auto basic_color_display =
    vbox(
      text(L"Basic Color Set:"),
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
        )
      )
    );

  // clang-format on
  auto palette_256_color_display = vbox(text(L"256 color palette:"));

  int y = -1;
  for (int i = 0; i < 256; ++i) {
    if (i % 16 == 0) {
      palette_256_color_display->children.push_back(hbox());
      ++y;
    }
    std::string number = std::to_string(i);
    while (number.length() < 4) {
      number.push_back(' ');
    }
    palette_256_color_display->children.back()->children.push_back(
        bgcolor(Color::Palette256(i), text(to_wstring(number))));
  }

  auto true_color_display = vbox(text(L"a true color grandient:"));

  for (int i = 0; i < 17; ++i) {
    true_color_display->children.push_back(hbox());

    for (int j = 0; j < 30; ++j) {
      true_color_display->children.back()->children.push_back(
          bgcolor(Color(50 + i * 5, 100 + j, 150), text(L" ")));
    }
  }

  auto document =
      vbox(terminal_info, text(L""),
           hbox(basic_color_display, text(L"  "), palette_256_color_display,
                text(L" "), true_color_display));
  // clang-format on

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
