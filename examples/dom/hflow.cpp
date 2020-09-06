#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto make_box = [](size_t dimx, size_t dimy) {
    std::wstring title = to_wstring(dimx) + L"x" + to_wstring(dimy);
    return window(text(title) | hcenter | bold,
                  text(L"content") | hcenter | dim) |
           size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
  };

  auto style = size(WIDTH, GREATER_THAN, 20) | border |
               size(HEIGHT, GREATER_THAN, 30) | size(WIDTH, LESS_THAN, 50);

  auto document = hflow({
                      make_box(7, 7),
                      make_box(7, 5),
                      make_box(5, 7),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(10, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(11, 4),
                      make_box(12, 4),
                      make_box(12, 5),
                      make_box(12, 4),
                      make_box(13, 4),
                      make_box(13, 3),
                      make_box(13, 3),
                      make_box(10, 3),
                  }) |
                  style;

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  std::cout << screen.ToString() << std::endl;

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
