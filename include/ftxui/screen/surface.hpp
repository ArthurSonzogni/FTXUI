// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_SURFACE_HPP
#define FTXUI_SCREEN_SURFACE_HPP

#include <string>  // for string, basic_string, allocator
#include <vector>  // for vector

#include "ftxui/screen/box.hpp"   // for Box
#include "ftxui/screen/cell.hpp"  // for Cell

namespace ftxui {

/// @brief A rectangular grid of Cell.
///
/// @note This class was previously named Image.
///
/// @ingroup screen
class Surface {
 public:
  // Constructors:
  Surface() = delete;
  Surface(int dimx, int dimy);

  // Destructor:
  virtual ~Surface() = default;

  // Access a character in the grid at a given position.
  std::string& at(int x, int y);
  const std::string& at(int x, int y) const;

  // Access a cell (Cell) in the grid at a given position.
  Cell& CellAt(int x, int y);
  const Cell& CellAt(int x, int y) const;

  // [Deprecated] alias for CellAt.
  Cell& PixelAt(int x, int y) { return CellAt(x, y); }
  const Cell& PixelAt(int x, int y) const { return CellAt(x, y); }

  // Get screen dimensions.
  int dimx() const { return dimx_; }
  int dimy() const { return dimy_; }

  // Fill the surface with space and default style
  void Clear();

  Box stencil;

 protected:
  int dimx_;
  int dimy_;
  std::vector<std::vector<Cell>> cells_;
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_SURFACE_HPP
