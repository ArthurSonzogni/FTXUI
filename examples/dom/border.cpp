// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  // clang-format off
  auto document =
    hbox(
      window(text(L" main frame ") | hcenter,
        vbox(
          text(L"Line 1"),
          text(L"Line 2"),
          text(L"Line 3"),
          vbox(
            text(L"Line 4"),
            text(L"Line 5"),
            text(L"Line 6")
          ) | border,
          hbox(
            window(text(L"frame 2"),
              vbox(
                text(L"Line 4"),
                text(L"Line 5"),
                text(L"Line 6")
              )
            ),
            window(text(L"frame 3"),
              vbox(
                text(L"Line 7"),
                text(L"Line 8"),
                text(L"Line 9")
              )
            )
          ),
          text(L"footer footer footer footer footer")
        )
      ),
      filler()
    );
  // clang-format on
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document.get());
  std::cout << screen.ToString() << std::endl;
}
