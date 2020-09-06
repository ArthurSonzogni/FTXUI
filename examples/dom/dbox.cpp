#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = dbox({
      vbox({
          text(L"line_1"),
          text(L"line_2"),
          text(L"line_3"),
          text(L"line_4"),
          text(L"line_5"),
      }) | border,
      text(L"overlay") | border | center,
  });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
