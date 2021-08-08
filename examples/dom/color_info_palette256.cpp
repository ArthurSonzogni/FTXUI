#include <ftxui/dom/elements.hpp>  // for bgcolor, hbox, operator|, Elements, vbox, Element
#include <ftxui/screen/color_info.hpp>  // for ColorInfo
#include <ftxui/screen/screen.hpp>      // for Dimension, Screen
#include <string>                       // for allocator, string
#include <utility>                      // for move
#include <vector>                       // for vector

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui
#include "ftxui/screen/color.hpp"    // for Color, Color::Palette256
#include "ftxui/screen/string.hpp"   // for to_wstring

using namespace ftxui;
#include "./color_info_sorted_2d.ipp"  // for ColorInfoSorted2D

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

  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
