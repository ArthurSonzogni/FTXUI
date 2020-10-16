#ifndef FTXUI_CORE_TERMINAL_HPP
#define FTXUI_CORE_TERMINAL_HPP

namespace ftxui {

class Terminal {
 public:
  struct Dimensions {
    int dimx;
    int dimy;
  };
  static Dimensions Size();

  enum Color {
    Palette1,
    Palette16,
    Palette256,
    TrueColor,
  };
  static Color ColorSupport();
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_CORE_TERMINAL_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
