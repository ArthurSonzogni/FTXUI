// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/dom/elements.hpp>  // for operator|, text, Element, hcenter, Fit, hbox, size, window, Elements, bold, dim, EQUAL, WIDTH
#include <ftxui/screen/screen.hpp>  // for Screen
#include <memory>                   // for allocator, shared_ptr
#include <string>                   // for string, to_string
#include <utility>                  // for move

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

int main() {
  using namespace ftxui;
  auto make_box = [](const std::string& title) {
    return window(text(title) | hcenter | bold,
                  text("content") | hcenter | dim);
  };

  Elements content;
  for (int x = 3; x < 30; ++x) {
    content.push_back(make_box(std::to_string(x)) | size(WIDTH, EQUAL, x));
  }

  auto document = hbox(std::move(content));
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}
