#include <cmath>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color_info.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <iostream>

#include "ftxui/screen/string.hpp"

using namespace ftxui;
#include "./color_info_sorted_2d.ipp"  // ColorInfoSorted2D.

int main(int argc, const char* argv[]) {
  // clang-format off
  auto basic_color_display =
    vbox(
      text(L"16 color palette:"),
      separator(),
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
  auto palette_256_color_display = text(L"256 colors palette:");
  {
    std::vector<std::vector<ColorInfo>> info_columns = ColorInfoSorted2D();
    Elements columns;
    for (auto& column : info_columns) {
      Elements column_elements;
      for (auto& it : column) {
        column_elements.push_back(
            text(L"   ") | bgcolor(Color(Color::Palette256(it.index_256))));
      }
      columns.push_back(hbox(std::move(column_elements)));
    }
    palette_256_color_display = vbox({
        palette_256_color_display,
        separator(),
        vbox(columns),
    });
  }

  // True color display.
  auto true_color_display = text(L"TrueColors: 24bits:");
  {
    int saturation = 255;
    Elements array;
    for (int value = 0; value < 255; value += 16) {
      Elements line;
      for (int hue = 0; hue < 255; hue += 6) {
        line.push_back(text(L"▀")                                   //
                       | color(Color::HSV(hue, saturation, value))  //
                       | bgcolor(Color::HSV(hue, saturation, value + 8)));
      }
      array.push_back(hbox(std::move(line)));
    }
    true_color_display = vbox({
        true_color_display,
        separator(),
        vbox(std::move(array)),
    });
  }

  auto terminal_info =
      vbox({
          Terminal::ColorSupport() >= Terminal::Color::Palette16
              ? text(L" 16 color palette support : Yes")
              : text(L" 16 color palette support : No"),
          Terminal::ColorSupport() >= Terminal::Color::Palette256
              ? text(L"256 color palette support : Yes")
              : text(L"256 color palette support : No"),
          Terminal::ColorSupport() >= Terminal::Color::TrueColor
              ? text(L"       True color support : Yes")
              : text(L"       True color support : No"),
      }) |
      border;

  auto document = vbox({hbox({
                            basic_color_display,
                            text(L" "),
                            palette_256_color_display,
                            text(L" "),
                            true_color_display,
                        }),
                        terminal_info});
  // clang-format on

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
