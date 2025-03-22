// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_PIXEL_HPP
#define FTXUI_SCREEN_PIXEL_HPP

#include <cstdint>                 // for uint8_t
#include <string>                  // for string, basic_string, allocator
#include "ftxui/screen/color.hpp"  // for Color, Color::Default

namespace ftxui {

/// @brief A Unicode character and its associated style.
/// @ingroup screen
struct Pixel {
  Pixel()
      : blink(false),
        bold(false),
        dim(false),
        italic(false),
        inverted(false),
        underlined(false),
        underlined_double(false),
        strikethrough(false),
        automerge(false) {}

  // A bit field representing the style:
  bool blink : 1;
  bool bold : 1;
  bool dim : 1;
  bool italic : 1;
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
  std::string character = "";

  // Colors:
  Color background_color = Color::Default;
  Color foreground_color = Color::Default;
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_PIXEL_HPP
