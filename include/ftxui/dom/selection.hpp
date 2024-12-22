// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_DOM_SELECTION_HPP
#define FTXUI_DOM_SELECTION_HPP

#include <functional>

#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/pixel.hpp"  // for Pixel

namespace ftxui {

/// @brief Represent a selection in the terminal.
class Selection {
 public:
  Selection(int start_x, int start_y, int end_x, int end_y);
  const Box& GetBox() const;

  Selection SaturateHorizontal(Box box);
  Selection SaturateVertical(Box box);

 private:
  Selection* const parent_ = nullptr;
  const int start_x_;
  const int start_y_;
  const int end_x_;
  const int end_y_;
  const Box box_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_SELECTION_HPP */
