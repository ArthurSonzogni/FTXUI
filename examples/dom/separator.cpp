#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document = hbox({
                      text(L"left-column"),
                      separator(),
                      vbox({
                          center(text(L"right-top")) | flex,
                          separator(),
                          center(text(L"bottom-bottom")),
                      }) | flex,
                      separator(),
                      text(L"right-column"),
                  }) |
                  border;
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString() << std::endl;

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
