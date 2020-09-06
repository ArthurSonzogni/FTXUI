#include <algorithm>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color_info.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <iostream>
#include "ftxui/screen/string.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  // Acquire the color information for the palette256.
  std::vector<ColorInfo> info_gray;
  std::vector<ColorInfo> info_color;
  for (int i = 0; i < 256; ++i) {
    ColorInfo info = GetColorInfo(Color::Palette256(i));
    if (info.saturation == 0)
      info_gray.push_back(info);
    else
      info_color.push_back(info);
  }

  // Sort info_color by hue.
  std::sort(
      info_color.begin(), info_color.end(),
      [](const ColorInfo& A, const ColorInfo& B) { return A.hue < B.hue; });

  // Make 8 colums, one gray and seven colored.
  std::vector<std::vector<ColorInfo>> info_columns(8);
  info_columns[0] = info_gray;
  for (int i = 0; i < info_color.size(); ++i) {
    info_columns[1 + 7 * i / info_color.size()].push_back(info_color[i]);
  }

  // Minimize discontinuities for every columns.
  for (auto& column : info_columns) {
    std::sort(column.begin(), column.end(),
              [](const ColorInfo& A, const ColorInfo& B) {
                return A.value < B.value;
              });
    for (int i = 0; i < column.size() - 1; ++i) {
      int best_index = i + 1;
      int best_distance = 255 * 255 * 3;
      for (int j = i + 1; j < column.size(); ++j) {
        int dx = column[i].red - column[j].red;
        int dy = column[i].green - column[j].green;
        int dz = column[i].blue - column[j].blue;
        int distance = dx * dx + dy * dy + dz * dz;
        if (best_distance > distance) {
          best_distance = distance;
          best_index = j;
        }
      }
      std::swap(column[i + 1], column[best_index]);
    }
  }

  // Draw every columns
  Elements columns_elements;
  for (auto& column : info_columns) {
    Elements column_elements;
    for (auto& it : column) {
      column_elements.push_back(hbox({
          text(L"     ") | bgcolor(Color(Color::Palette256(it.index))),
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
