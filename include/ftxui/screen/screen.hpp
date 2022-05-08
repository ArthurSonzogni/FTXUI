#ifndef FTXUI_SCREEN_SCREEN_HPP
#define FTXUI_SCREEN_SCREEN_HPP

#include <memory>
#include <string>  // for string, allocator, basic_string
#include <vector>  // for vector

#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/color.hpp"     // for Color, Color::Default
#include "ftxui/screen/terminal.hpp"  // for Dimensions

namespace ftxui {

/// @brief A unicode character and its associated style.
/// @ingroup screen
struct Pixel {
  bool operator==(const Pixel& other) const;

  // The graphemes stored into the pixel. To support combining characters,
  // like: a⃦, this can potentially contains multiple codepoitns.
  std::string character = " ";

  // Colors:
  Color background_color = Color::Default;
  Color foreground_color = Color::Default;

  // A bit field representing the style:
  bool blink : 1;
  bool bold : 1;
  bool dim : 1;
  bool inverted : 1;
  bool underlined : 1;
  bool automerge : 1;

  Pixel()
      : blink(false),
        bold(false),
        dim(false),
        inverted(false),
        underlined(false),
        automerge(false) {}
};

/// @brief Define how the Screen's dimensions should look like.
/// @ingroup screen
namespace Dimension {
Dimensions Fixed(int);
Dimensions Full();
}  // namespace Dimension

/// @brief A rectangular grid of Pixel.
/// @ingroup screen
class Screen {
 public:
  // Constructors:
  Screen(int dimx, int dimy);
  static Screen Create(Dimensions dimension);
  static Screen Create(Dimensions width, Dimensions height);

  // Node write into the screen using Screen::at.
  std::string& at(int x, int y);
  Pixel& PixelAt(int x, int y);

  // Convert the screen into a printable string in the terminal.
  std::string ToString();
  void Print();

  // Get screen dimensions.
  int dimx() const { return dimx_; }
  int dimy() const { return dimy_; }

  // Move the terminal cursor n-lines up with n = dimy().
  std::string ResetPosition(bool clear = false) const;

  // Fill with space.
  void Clear();

  void ApplyShader();

  struct Cursor {
    int x = 0;
    int y = 0;
  };
  Cursor cursor() const { return cursor_; }
  void SetCursor(Cursor cursor) { cursor_ = cursor; }

  Box stencil;

 protected:
  int dimx_;
  int dimy_;
  std::vector<std::vector<Pixel>> pixels_;
  Cursor cursor_;

 private:
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_SCREEN_HPP

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
