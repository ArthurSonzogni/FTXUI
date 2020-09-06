#include "ftxui/screen/color.hpp"

#include <algorithm>

#include "ftxui/screen/string.hpp"

namespace ftxui {

bool Color::operator==(const Color& rhs) const {
  return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ &&
         type_ == rhs.type_;
}

bool Color::operator!=(const Color& rhs) const {
  return !operator==(rhs);
}

std::wstring Color::Print(bool is_background_color) const {
  switch (type_) {
    case ColorType::Palette16:
      return to_wstring(
          std::to_string((is_background_color ? 10 : 0) + index_));

    case ColorType::Palette256:
      return to_wstring(std::to_string(is_background_color ? 48 : 38)  //
                        + ";5;"                                        //
                        + std::to_string(index_));                     //

    case ColorType::TrueColor:
      return to_wstring(std::to_string(is_background_color ? 48 : 38)  //
                        + ";2;"                                        //
                        + std::to_string(red_) + ";"                   //
                        + std::to_string(green_) + ";"                 //
                        + std::to_string(blue_));                      //
  }
  return L"";
}

/// @brief Build a transparent color.
/// @ingroup screen
Color::Color() : type_(ColorType::Palette16), index_(Palette16::Default) {}

/// @brief Build a transparent using Palette16 colors.
/// @ingroup screen
Color::Color(Palette16 index) : type_(ColorType::Palette16), index_(index) {}

/// @brief Build a transparent using Palette256 colors.
/// @ingroup screen
Color::Color(Palette256 index) : type_(ColorType::Palette256), index_(index) {}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @ingroup screen
Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : type_(ColorType::TrueColor), red_(red), green_(green), blue_(blue) {}

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
/// @param hue The hue of the color [0,255]
/// @param saturation The "colorfulness" [0,255].
/// @param value The "Lightness" [0,255]
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

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
