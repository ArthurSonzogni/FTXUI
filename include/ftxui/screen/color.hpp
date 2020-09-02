#ifndef FTXUI_SCREEN_COLOR
#define FTXUI_SCREEN_COLOR

#include <cstdint>
#include <string>

namespace ftxui {

/// @brief Enum indicating to the terminal how to interpret and display the
/// color.
/// @ingroup screen
enum class ColorType : uint8_t {
  None,
  Basic,
  Palette256,
  TrueColor,
};

/// @brief A class representing terminal colors.
/// @ingroup screen
class Color {
 public:
  // --- Static instances representing the basic color set -----

  // --- Transparent -----
  static Color Default;

  // --- Grayscale -----
  static Color Black;
  static Color GrayDark;
  static Color GrayLight;
  static Color White;

  // --- Hue -----
  static Color Blue;
  static Color BlueLight;

  static Color Cyan;
  static Color CyanLight;

  static Color Green;
  static Color GreenLight;

  static Color Magenta;
  static Color MagentaLight;

  static Color Red;
  static Color RedLight;

  static Color Yellow;
  static Color YellowLight;

 public:
  static Color Color256(int index);
  static Color ColorRGB(int r, int g, int b);

  // --- Operators ------
  bool operator==(const Color& rhs) const;
  bool operator!=(const Color& rhs) const;

  std::wstring ToTerminalColorCode(bool is_background_color) const;

 protected:
  // prefer available static instances if you want to use a basic color
  static Color ColorBasic(int index);

  ColorType type_ = ColorType::None;
  int index_ = -1;
  int r_ = -1;
  int g_ = -1;
  int b_ = -1;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COLOR_H_ */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
