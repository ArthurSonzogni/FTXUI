// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <sstream>  // IWYU pragma: keep
#include <string>
#include <vector>

#include "ftxui/screen/surface.hpp"
#include "ftxui/screen/cell.hpp"

namespace ftxui {

namespace {
Cell& dev_null_cell() {
  static Cell cell;
  return cell;
}
}  // namespace

Surface::Surface(int dimx, int dimy)
    : stencil{0, dimx - 1, 0, dimy - 1},
      dimx_(dimx),
      dimy_(dimy),
      cells_(dimy, std::vector<Cell>(dimx)) {}

/// @brief Access a character in a cell at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
std::string& Surface::at(int x, int y) {
  return CellAt(x, y).character;
}

/// @brief Access a character in a cell at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
const std::string& Surface::at(int x, int y) const {
  return CellAt(x, y).character;
}

/// @brief Access a cell (Cell) at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
Cell& Surface::CellAt(int x, int y) {
  return stencil.Contain(x, y) ? cells_[y][x] : dev_null_cell();
}

/// @brief Access a cell (Cell) at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
const Cell& Surface::CellAt(int x, int y) const {
  return stencil.Contain(x, y) ? cells_[y][x] : dev_null_cell();
}

/// @brief Clear all the cells from the surface.
void Surface::Clear() {
  for (auto& line : cells_) {
    for (auto& cell : line) {
      cell = Cell();
    }
  }
}

}  // namespace ftxui
