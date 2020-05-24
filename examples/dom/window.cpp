// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = window(text(L"Title"), text(L"content"));
  auto screen = Screen::Create(Dimension::Fixed(30), Dimension::Fixed(6));
  Render(screen, document);
  std::cout << screen.ToString() << std::endl;
}
