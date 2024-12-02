// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_DOM_SELECTION_HPP
#define FTXUI_DOM_SELECTION_HPP

#include <functional>

#include "ftxui/screen/box.hpp"  // for Box
#include "ftxui/screen/pixel.hpp"  // for Pixel

namespace ftxui {

/// @brief Option for the selection of content.
/// @ingroup component
struct SelectionOption {
  /// @brief Selection is simply inverted:
  static SelectionOption Simple();

  // Style:
  std::function<void(Pixel& pixel)> transform ;

  // Observers:
  /// Called when the selection changed.
  std::function<void()> on_change = [] {};
};

/// @brief Represent a selection in the terminal.
class Selection {
 public:
  Selection(int start_x, int start_y, int end_x, int end_y, SelectionOption option = SelectionOption::Simple());
  const Box& GetBox() const;
  const SelectionOption& GetOption() const;

  Selection SaturateHorizontal(Box box);
  Selection SaturateVertical(Box box);

 private:
  Selection* const parent_ = nullptr;
  const int start_x_;
  const int start_y_;
  const int end_x_;
  const int end_y_;
  const Box box_;
  const SelectionOption option;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_SELECTION_HPP */
