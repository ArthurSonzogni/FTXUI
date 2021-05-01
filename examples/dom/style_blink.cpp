#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>

#include "ftxui/dom/node.hpp"
#include "ftxui/screen/box.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document =  //
      hbox({
          text(L"This text is "),
          text(L"blink") | blink,
          text(L". Do you like it?"),
      });
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
