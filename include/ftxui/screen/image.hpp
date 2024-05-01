// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_IMAGE_HPP
#define FTXUI_SCREEN_IMAGE_HPP

#include <string>  // for string, basic_string, allocator
#include <vector>  // for vector

#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/pixel.hpp"  // for Pixel

namespace ftxui {

/// @brief A rectangular grid of Pixel.
/// @ingroup screen
class Image {
 public:
  // Constructors:
  Image() = delete;
  Image(int dimx, int dimy);

  // Access a character in the grid at a given position.
  std::string& at(int x, int y);
  const std::string& at(int x, int y) const;

  // Access a cell (Pixel) in the grid at a given position.
  Pixel& PixelAt(int x, int y);
  const Pixel& PixelAt(int x, int y) const;

  // Get screen dimensions.
  int dimx() const { return dimx_; }
  int dimy() const { return dimy_; }

  // Fill the image with space and default style
  void Clear();

  Box stencil;

 protected:
  int dimx_;
  int dimy_;
  std::vector<std::vector<Pixel>> pixels_;
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_IMAGE_HPP
