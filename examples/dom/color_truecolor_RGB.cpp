#include <cmath>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <iostream>

#include "ftxui/screen/string.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  int saturation = 255;
  Elements red_line;
  Elements green_line;
  Elements blue_line;
  Elements cyan_line;
  Elements magenta_line;
  Elements yellow_line;

  for (int value = 0; value < 255; value += 3) {
    int v = value * value / 255;
    red_line.push_back(text(L" ") | bgcolor(Color::RGB(v, 0, 0)));
    green_line.push_back(text(L" ") | bgcolor(Color::RGB(0, v, 0)));
    blue_line.push_back(text(L" ") | bgcolor(Color::RGB(0, 0, v)));
    cyan_line.push_back(text(L" ") | bgcolor(Color::RGB(0, v, v)));
    magenta_line.push_back(text(L" ") | bgcolor(Color::RGB(v, 0, v)));
    yellow_line.push_back(text(L" ") | bgcolor(Color::RGB(v, v, 0)));
  }

  auto document = vbox({
      window(text(L"Primary colors"),
             vbox({
                 hbox({text(L"Red line    :"), hbox(std::move(red_line))}),
                 hbox({text(L"Green line  :"), hbox(std::move(green_line))}),
                 hbox({text(L"Blue line   :"), hbox(std::move(blue_line))}),
             })),
      window(text(L"Secondary colors"),
             vbox({
                 hbox({text(L"cyan line   :"), hbox(std::move(cyan_line))}),
                 hbox({text(L"magenta line:"), hbox(std::move(magenta_line))}),
                 hbox({text(L"Yellow line :"), hbox(std::move(yellow_line))}),
             })),
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
