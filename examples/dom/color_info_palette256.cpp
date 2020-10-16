#include <algorithm>
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
  std::vector<std::vector<ColorInfo>> info_columns = ColorInfoSorted2D();

  // Draw every columns
  Elements columns_elements;
  for (auto& column : info_columns) {
    Elements column_elements;
    for (auto& it : column) {
      column_elements.push_back(hbox({
          text(L"     ") | bgcolor(Color(Color::Palette256(it.index_256))),
          text(to_wstring(std::string(it.name))),
      }));
    }
    columns_elements.push_back(vbox(std::move(column_elements)));
  }
  auto document = hbox(std::move(columns_elements));

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
