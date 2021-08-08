#include <stdio.h>                  // for getchar
#include <ftxui/dom/elements.hpp>   // for filler, hbox, vbox
#include <ftxui/screen/screen.hpp>  // for Screen, Dimension
#include <memory>                   // for allocator

#include "ftxui/dom/deprecated.hpp"  // for text
#include "ftxui/dom/node.hpp"        // for Render
#include "ftxui/screen/box.hpp"      // for ftxui

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto document =  //
      vbox({
          hbox({
              text(L"north-west"),
              filler(),
              text(L"north-east"),
          }),
          filler(),
          hbox({
              filler(),
              text(L"center"),
              filler(),
          }),
          filler(),
          hbox({
              text(L"south-west"),
              filler(),
              text(L"south-east"),
          }),
      });
  auto screen = Screen::Create(Dimension::Full());
  Render(screen, document);
  screen.Print();
  getchar();

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
