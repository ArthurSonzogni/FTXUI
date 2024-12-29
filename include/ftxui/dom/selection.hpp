// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_DOM_SELECTION_HPP
#define FTXUI_DOM_SELECTION_HPP

#include <functional>

#include <sstream>
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/pixel.hpp"  // for Pixel

namespace ftxui {

/// @brief Represent a selection in the terminal.
class Selection {
 public:
  Selection();  // Empty selection.
  Selection(int start_x, int start_y, int end_x, int end_y);

  const Box& GetBox() const;

  Selection SaturateHorizontal(Box box);
  Selection SaturateVertical(Box box);
  bool IsEmpty() const { return empty_; }

  void AddPart(const std::string& part, int y, int left, int right);
  std::string GetParts() { return parts_.str(); }

 private:
  Selection(int start_x, int start_y, int end_x, int end_y, Selection* parent);

  const int start_x_ = 0;
  const int start_y_ = 0;
  const int end_x_ = 0;
  const int end_y_ = 0;
  const Box box_ = {};
  Selection* const parent_ = this;
  const bool empty_ = true;
  std::stringstream parts_;

  // The position of the last inserted part.
  int x_ = 0;
  int y_ = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_SELECTION_HPP */
