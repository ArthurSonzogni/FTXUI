// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_SCREEN_HPP
#define FTXUI_SCREEN_SCREEN_HPP

#include <cstdint>     // for uint8_t
#include <functional>  // for function
#include <string>      // for string, basic_string, allocator
#include <vector>      // for vector

#include "ftxui/screen/image.hpp"     // for Pixel, Image
#include "ftxui/screen/terminal.hpp"  // for Dimensions
#include "ftxui/util/autoreset.hpp"   // for AutoReset

namespace ftxui {

/// @brief Define how the Screen's dimensions should look like.
/// @ingroup screen
namespace Dimension {
Dimensions Fixed(int);
Dimensions Full();
}  // namespace Dimension

/// @brief A rectangular grid of Pixel.
/// @ingroup screen
class Screen : public Image {
 public:
  // Constructors:
  Screen(int dimx, int dimy);
  static Screen Create(Dimensions dimension);
  static Screen Create(Dimensions width, Dimensions height);

  std::string ToString() const;

  // Print the Screen on to the terminal.
  void Print() const;

  // Fill the screen with space and reset any screen state, like hyperlinks, and
  // cursor
  void Clear();

  // Move the terminal cursor n-lines up with n = dimy().
  std::string ResetPosition(bool clear = false) const;

  void ApplyShader();

  struct Cursor {
    int x = 0;
    int y = 0;

    enum Shape {
      Hidden = 0,
      BlockBlinking = 1,
      Block = 2,
      UnderlineBlinking = 3,
      Underline = 4,
      BarBlinking = 5,
      Bar = 6,
    };
    Shape shape;
  };

  Cursor cursor() const { return cursor_; }
  void SetCursor(Cursor cursor) { cursor_ = cursor; }

  // Store an hyperlink in the screen. Return the id of the hyperlink. The id is
  // used to identify the hyperlink when the user click on it.
  uint8_t RegisterHyperlink(const std::string& link);
  const std::string& Hyperlink(uint8_t id) const;

  using SelectionStyle = std::function<void(Pixel&)>;
  const SelectionStyle& GetSelectionStyle() const;
  void SetSelectionStyle(SelectionStyle decorator);

 protected:
  Cursor cursor_;
  std::vector<std::string> hyperlinks_ = {""};

  // The current selection style. This is overridden by various dom elements.
  SelectionStyle selection_style_ = [](Pixel& pixel) {
    pixel.inverted ^= true;
  };
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_SCREEN_HPP
