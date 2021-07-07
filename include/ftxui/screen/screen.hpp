#ifndef FTXUI_SCREEN_SCREEN
#define FTXUI_SCREEN_SCREEN

#include <memory>
#include <string>
#include <vector>

#include "ftxui/screen/box.hpp"
#include "ftxui/screen/color.hpp"

namespace ftxui {
class Node;
}

namespace ftxui {
using Element = std::shared_ptr<Node>;

/// @brief A unicode character and its associated style.
/// @ingroup screen
struct Pixel {
  // The graphemes stored into the pixel. To support combining characters,
  // like: a⃦, this can potentially contains multiple codepoitns.
  // Required: character.size() >= 1;
  std::wstring character = L" ";

  // Colors:
  Color background_color = Color::Default;
  Color foreground_color = Color::Default;

  // A bit field representing the style:
  bool blink : 1;
  bool bold : 1;
  bool dim : 1;
  bool inverted : 1;
  bool underlined : 1;

  Pixel()
      : blink(false),
        bold(false),
        dim(false),
        inverted(false),
        underlined(false) {}
};

/// @brief Define how the Screen's dimensions should look like.
/// @ingroup screen
struct Dimension {
  /// coucou
  static Dimension Fixed(int);
  /// @brief coucou
  static Dimension Fit(Element&);
  static Dimension Full();

  int dimx;
  int dimy;
};

/// @brief A rectangular grid of Pixel.
/// @ingroup screen
class Screen {
 public:
  // Constructors:
  Screen(int dimx, int dimy);
  static Screen Create(Dimension dimension);
  static Screen Create(Dimension width, Dimension height);

  // Node write into the screen using Screen::at.
  wchar_t& at(int x, int y);
  Pixel& PixelAt(int x, int y);

  // Convert the screen into a printable string in the terminal.
  std::string ToString();
  void Print();

  // Get screen dimensions.
  int dimx() { return dimx_; }
  int dimy() { return dimy_; }

  // Move the terminal cursor n-lines up with n = dimy().
  std::string ResetPosition(bool clear = false);

  // Fill with space.
  void Clear();

  void ApplyShader();
  Box stencil;

  struct Cursor {
    int x = 0;
    int y = 0;
  };
  Cursor cursor() const { return cursor_; }
  void SetCursor(Cursor cursor) { cursor_ = cursor; }

 protected:
  int dimx_;
  int dimy_;
  std::vector<std::vector<Pixel>> pixels_;
  Cursor cursor_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_SCREEN */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
