// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cstdint>  // for size_t
#include <iostream>  // for operator<<, stringstream, basic_ostream, flush, cout, ostream
#include <limits>
#include <map>      // for _Rb_tree_const_iterator, map, operator!=, operator==
#include <memory>   // for allocator, allocator_traits<>::value_type
#include <sstream>  // IWYU pragma: keep
#include <utility>  // for pair

#include "ftxui/screen/image.hpp"
#include "ftxui/screen/string.hpp"    // for string_width

namespace ftxui {

namespace
{
   Pixel& dev_null_pixel() {
      static Pixel pixel;
      return pixel;
   }
}

Image::Image(int dimx, int dimy)
    : stencil{0, dimx - 1, 0, dimy - 1},
      dimx_(dimx),
      dimy_(dimy),
      pixels_(dimy, std::vector<Pixel>(dimx)) {}

/// @brief Access a character in a cell at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
std::string& Image::at(int x, int y) {
  return PixelAt(x, y).character;
}

/// @brief Access a character in a cell at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
const std::string& Image::at(int x, int y) const {
  return PixelAt(x, y).character;
}

/// @brief Access a cell (Pixel) at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
Pixel& Image::PixelAt(int x, int y) {
  return stencil.Contain(x, y) ? pixels_[y][x] : dev_null_pixel();
}

/// @brief Access a cell (Pixel) at a given position.
/// @param x The cell position along the x-axis.
/// @param y The cell position along the y-axis.
const Pixel& Image::PixelAt(int x, int y) const {
  return stencil.Contain(x, y) ? pixels_[y][x] : dev_null_pixel();
}

/// @brief Clear all the pixel from the screen.
void Image::Clear() {
  for (auto& line : pixels_) {
    for (auto& cell : line) {
      cell = Pixel();
    }
  }
}

}  // namespace ftxui
