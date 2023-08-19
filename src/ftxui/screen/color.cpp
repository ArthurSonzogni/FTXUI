// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/color.hpp"

#include <array>  // for array
#include <cmath>
#include <cstdint>
#include <string_view>  // for literals

#include "ftxui/screen/color_info.hpp"  // for GetColorInfo, ColorInfo
#include "ftxui/screen/terminal.hpp"  // for ColorSupport, Color, Palette256, TrueColor

namespace ftxui {

using namespace std::literals;

namespace {
const std::array<const char*, 33> palette16code = {
    "30", "40",   //
    "31", "41",   //
    "32", "42",   //
    "33", "43",   //
    "34", "44",   //
    "35", "45",   //
    "36", "46",   //
    "37", "47",   //
    "90", "100",  //
    "91", "101",  //
    "92", "102",  //
    "93", "103",  //
    "94", "104",  //
    "95", "105",  //
    "96", "106",  //
    "97", "107",  //
};

}  // namespace

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
      return is_background_color ? "49"s : "39"s;

    case ColorType::Palette16:
      return palette16code[2 * red_ + is_background_color];  // NOLINT;

    case ColorType::Palette256:
      return (is_background_color ? "48;5;"s : "38;5;"s) + std::to_string(red_);

    case ColorType::TrueColor:
    default:
      return (is_background_color ? "48;2;"s : "38;2;"s)  //
             + std::to_string(red_) + ";"                 //
             + std::to_string(green_) + ";"               //
             + std::to_string(blue_);                     //
  }
}

/// @brief Build a transparent color.
/// @ingroup screen
Color::Color() = default;

/// @brief Build a transparent color.
/// @ingroup screen
Color::Color(Palette1 /*value*/) : Color() {}

/// @brief Build a transparent using Palette16 colors.
/// @ingroup screen
Color::Color(Palette16 index) : type_(ColorType::Palette16), red_(index) {}

/// @brief Build a transparent using Palette256 colors.
/// @ingroup screen
Color::Color(Palette256 index) : type_(ColorType::Palette256), red_(index) {
  if (Terminal::ColorSupport() >= Terminal::Color::Palette256) {
    return;
  }
  type_ = ColorType::Palette16;
  red_ = GetColorInfo(Color::Palette256(red_)).index_16;
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
  if (Terminal::ColorSupport() == Terminal::Color::TrueColor) {
    return;
  }

  // Find the closest Color from the database:
  const int max_distance = 256 * 256 * 3;
  int closest = max_distance;
  int best = 0;
  const int database_begin = 16;
  const int database_end = 256;
  for (int i = database_begin; i < database_end; ++i) {
    const ColorInfo color_info = GetColorInfo(Color::Palette256(i));
    const int dr = color_info.red - red;
    const int dg = color_info.green - green;
    const int db = color_info.blue - blue;
    const int dist = dr * dr + dg * dg + db * db;
    if (closest > dist) {
      closest = dist;
      best = i;
    }
  }

  if (Terminal::ColorSupport() == Terminal::Color::Palette256) {
    type_ = ColorType::Palette256;
    red_ = best;
  } else {
    type_ = ColorType::Palette16;
    red_ = GetColorInfo(Color::Palette256(best)).index_16;
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
  return {red, green, blue};
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
  if (s == 0) {
    return {0, 0, 0};
  }

  uint8_t region = h / 43;                                        // NOLINT
  uint8_t remainder = (h - (region * 43)) * 6;                    // NOLINT
  uint8_t p = (v * (255 - s)) >> 8;                               // NOLINT
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;          // NOLINT
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;  // NOLINT

  // clang-format off
  switch (region) {              // NOLINT
    case 0: return Color(v,t,p); // NOLINT
    case 1: return Color(q,v,p); // NOLINT
    case 2: return Color(p,v,t); // NOLINT
    case 3: return Color(p,q,v); // NOLINT
    case 4: return Color(t,p,v); // NOLINT
    case 5: return Color(v,p,q); // NOLINT
  }                              // NOLINT
  // clang-format on
  return {0, 0, 0};
}

// static
Color Color::Interpolate(float t, const Color& a, const Color& b) {
  if (a.type_ == ColorType::Palette1 ||  //
      b.type_ == ColorType::Palette1) {
    if (t < 0.5F) {  // NOLINT
      return a;
    } else {
      return b;
    }
  }

  auto get_color = [](const Color& color,  //
                      uint8_t* red, uint8_t* green, uint8_t* blue) {
    switch (color.type_) {
      case ColorType::Palette1: {
        return;
      }

      case ColorType::Palette16: {
        const ColorInfo info = GetColorInfo(Color::Palette16(color.red_));
        *red = info.red;
        *green = info.green;
        *blue = info.blue;
        return;
      }

      case ColorType::Palette256: {
        const ColorInfo info = GetColorInfo(Color::Palette256(color.red_));
        *red = info.red;
        *green = info.green;
        *blue = info.blue;
        return;
      }

      case ColorType::TrueColor:
      default: {
        *red = color.red_;
        *green = color.green_;
        *blue = color.blue_;
        return;
      }
    }
  };

  uint8_t a_r = 0;
  uint8_t a_g = 0;
  uint8_t a_b = 0;
  uint8_t b_r = 0;
  uint8_t b_g = 0;
  uint8_t b_b = 0;
  get_color(a, &a_r, &a_g, &a_b);
  get_color(b, &b_r, &b_g, &b_b);

  // Gamma correction:
  // https://en.wikipedia.org/wiki/Gamma_correction
  auto interp = [t](uint8_t a_u, uint8_t b_u) {
    constexpr float gamma = 2.2F;
    const float a_f = powf(a_u, gamma);
    const float b_f = powf(b_u, gamma);
    const float c_f = a_f * (1.0F - t) +  //
                      b_f * t;
    return static_cast<uint8_t>(powf(c_f, 1.F / gamma));
  };
  return Color::RGB(interp(a_r, b_r),   //
                    interp(a_g, b_g),   //
                    interp(a_b, b_b));  //
}

inline namespace literals {

Color operator""_rgb(unsigned long long int combined) {
  // assert(combined <= 0xffffffU);
  auto const red = static_cast<uint8_t>(combined >> 16U);
  auto const green = static_cast<uint8_t>(combined >> 8U);
  auto const blue = static_cast<uint8_t>(combined);
  return {red, green, blue};
}

}  // namespace literals

}  // namespace ftxui
