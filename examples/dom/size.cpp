#include <ftxui/dom/elements.hpp>  // for operator|, Element, hcenter, hbox, size, window, Elements, bold, dim, EQUAL, WIDTH
#include <ftxui/screen/screen.hpp>  // for Screen, Dimension
#include <ftxui/screen/string.hpp>  // for to_wstring
#include <memory>                   // for allocator, shared_ptr
#include <string>                   // for wstring
#include <utility>                  // for move

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto make_box = [](const std::wstring title) {
    return window(text(title) | hcenter | bold,
                  text(L"content") | hcenter | dim);
  };

  Elements content;
  for (int x = 3; x < 30; ++x) {
    content.push_back(make_box(to_wstring(x)) | size(WIDTH, EQUAL, x));
  }

  auto document = hbox(std::move(content));
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
