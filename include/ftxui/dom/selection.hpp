// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_DOM_SELECTION_HPP
#define FTXUI_DOM_SELECTION_HPP

#include <functional>

#include <sstream>
#include "ftxui/screen/box.hpp"   // for Box
#include "ftxui/screen/cell.hpp"  // for Cell
#include "ftxui/util/export.hpp"  // for FTXUI_EXPORT

namespace ftxui {

/// @brief Represents a selection in a terminal user interface.
///
/// Selection is a class that represents the two endpoints of a selection in a
/// terminal user interface.
///
/// @ingroup dom
class FTXUI_EXPORT(DOM) Selection {
 public:
  /// Create an empty selection.
  Selection();
  /// Create a selection spanning from (start_x, start_y) to (end_x, end_y).
  Selection(int start_x, int start_y, int end_x, int end_y);

  /// Return the bounding box covering the selection endpoints.
  const Box& GetBox() const;

  /// Restrict and adapt the selection horizontally to `box`.
  Selection SaturateHorizontal(Box box);
  /// Restrict and adapt the selection vertically to `box`.
  Selection SaturateVertical(Box box);
  /// Return true if no region is selected.
  bool IsEmpty() const { return empty_; }

  /// Append a selected text fragment at row `y` spanning [left, right].
  void AddPart(std::string_view part, int y, int left, int right);
  /// Return the accumulated selected text.
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
