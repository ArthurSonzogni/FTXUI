#include "ftxui/screen/color.hpp"

#include <cassert>

#include "ftxui/screen/color_info.hpp"  // for GetColorInfo, ColorInfo
#include "ftxui/screen/terminal.hpp"  // for Terminal, Terminal::Color, Terminal::Palette256, Terminal::TrueColor

namespace ftxui {

namespace {
const char* palette16code[16][2] = {
    {"30", "40"},  {"31", "41"},  {"32", "42"},  {"33", "43"},
    {"34", "44"},  {"35", "45"},  {"36", "46"},  {"37", "47"},
    {"90", "100"}, {"91", "101"}, {"92", "102"}, {"93", "103"},
    {"94", "104"}, {"95", "105"}, {"96", "106"}, {"97", "107"},
};
}

bool Color::operator==(const Color& rhs) const {
  return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ &&
         type_ == rhs.type_;
}

bool Color::operator!=(const Color& rhs) const {
  return !operator==(rhs);
}

std::string Color::Print(bool is_background_color) const {
  switch (type_) {
    case ColorType::Palette1:
      return is_background_color ? "49" : "39";

    case ColorType::Palette16:
      return palette16code[index_][is_background_color];

    case ColorType::Palette256:
      return (is_background_color ? "48;5;" : "38;5;") + std::to_string(index_);

    case ColorType::TrueColor:
      return (is_background_color ? "48;2;" : "38;2;")  //
             + std::to_string(red_) + ";"               //
             + std::to_string(green_) + ";"             //
             + std::to_string(blue_);                   //
  }
  return "";
}

/// @brief Build a transparent color.
/// @ingroup screen
Color::Color() : type_(ColorType::Palette1) {}

/// @brief Build a transparent color.
/// @ingroup screen
Color::Color(Palette1) : type_(ColorType::Palette1) {}

/// @brief Build a transparent using Palette16 colors.
/// @ingroup screen
Color::Color(Palette16 index) : type_(ColorType::Palette16), index_(index) {}

/// @brief Build a transparent using Palette256 colors.
/// @ingroup screen
Color::Color(Palette256 index) : type_(ColorType::Palette256), index_(index) {
  if (Terminal::ColorSupport() >= Terminal::Color::Palette256)
    return;
  type_ = ColorType::Palette16;
  index_ = GetColorInfo(Color::Palette256(index_)).index_16;
}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @ingroup screen
Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : type_(ColorType::TrueColor), red_(red), green_(green), blue_(blue) {
  if (Terminal::ColorSupport() == Terminal::Color::TrueColor)
    return;

  int closest = 256 * 256 * 3;
  int best = 0;
  for (int i = 16; i < 256; ++i) {
    ColorInfo color_info = GetColorInfo(Color::Palette256(i));
    int dr = color_info.red - red;
    int dg = color_info.green - green;
    int db = color_info.blue - blue;
    int dist = dr * dr + dg * dg + db * db;
    if (closest > dist) {
      closest = dist;
      best = i;
    }
  }

  if (Terminal::ColorSupport() == Terminal::Color::Palette256) {
    type_ = ColorType::Palette256;
    index_ = best;
  } else {
    type_ = ColorType::Palette16;
    index_ = GetColorInfo(Color::Palette256(best)).index_16;
  }
}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @ingroup screen
// static
Color Color::RGB(uint8_t red, uint8_t green, uint8_t blue) {
  return Color(red, green, blue);
}

/// @brief Build a Color from its HSV representation.
/// https://en.wikipedia.org/wiki/HSL_and_HSV
///
/// @param h The hue of the color [0,255]
/// @param s The "colorfulness" [0,255].
/// @param v The "Lightness" [0,255]
/// @ingroup screen
// static
Color Color::HSV(uint8_t h, uint8_t s, uint8_t v) {
  if (s == 0)
    return Color(v, v, v);

  uint8_t region = h / 43;
  uint8_t remainder = (h - (region * 43)) * 6;
  uint8_t p = (v * (255 - s)) >> 8;
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  // clang-format off
  switch (region) {
    case 0: return Color(v,t,p);
    case 1: return Color(q,v,p);
    case 2: return Color(p,v,t);
    case 3: return Color(p,q,v);
    case 4: return Color(t,p,v);
    case 5: return Color(v,p,q);
  }
  // clang-format on

  return Color(0, 0, 0);
}

inline namespace literals {

Color operator""_rgb(unsigned long long int combined) {
  assert(combined <= 0xffffffU);
  auto const red = static_cast<uint8_t>(combined >> 16);
  auto const green = static_cast<uint8_t>(combined >> 8);
  auto const blue = static_cast<uint8_t>(combined);
  return Color(red, green, blue);
}

}  // namespace literals

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
