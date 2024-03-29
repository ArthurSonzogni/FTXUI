// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_IMAGE_HPP
#define FTXUI_SCREEN_IMAGE_HPP

#include <cstdint>  // for uint8_t
#include <memory>
#include <string>  // for string, basic_string, allocator
#include <vector>  // for vector

#include "ftxui/screen/color.hpp"     // for Color, Color::Default
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {

/// @brief A unicode character and its associated style.
/// @ingroup screen
struct Pixel {
  Pixel()
      : blink(false),
        bold(false),
        dim(false),
        inverted(false),
        underlined(false),
        underlined_double(false),
        strikethrough(false),
        automerge(false) {}

  // A bit field representing the style:
  bool blink : 1;
  bool bold : 1;
  bool dim : 1;
  bool inverted : 1;
  bool underlined : 1;
  bool underlined_double : 1;
  bool strikethrough : 1;
  bool automerge : 1;

  // The hyperlink associated with the pixel.
  // 0 is the default value, meaning no hyperlink.
  // It's an index for accessing Screen meta data
  uint8_t hyperlink = 0;

  // The graphemes stored into the pixel. To support combining characters,
  // like: a?, this can potentially contain multiple codepoints.
  std::string character = " ";

  // Colors:
  Color background_color = Color::Default;
  Color foreground_color = Color::Default;
};

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
