// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/screen/color.hpp"

#include <algorithm>  // for clamp
#include <array>      // for array
#include <cmath>
#include <cstdint>
#include <cstdio>  // for snprintf
#include <string>

#include "ftxui/screen/color_info.hpp"  // for GetColorInfo, ColorInfo
#include "ftxui/screen/terminal.hpp"  // for ColorSupport, Color, Palette256, TrueColor

#if defined(_MSC_VER)
#define FTXUI_NOINLINE __declspec(noinline)
#else
#define FTXUI_NOINLINE __attribute__((noinline))
#endif

namespace ftxui {
namespace {
const std::array<const char*, 32> palette16code = {
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

void AppendNumber(std::string& out, uint8_t n) {
  if (n >= 100) {
    out += static_cast<char>('0' + n / 100);
    n %= 100;
    out += static_cast<char>('0' + n / 10);
    n %= 10;
    out += static_cast<char>('0' + n);
  } else if (n >= 10) {
    out += static_cast<char>('0' + n / 10);
    n %= 10;
    out += static_cast<char>('0' + n);
  } else {
    out += static_cast<char>('0' + n);
  }
}

// Index of the Palette256 color closest to (r, g, b), matching an exhaustive
// search over [16, 255]: ties resolve to the lowest index.
uint8_t ClosestPalette256(int r, int g, int b) {
  // Colors [16, 231] form a 6x6x6 cube of levels {0, 95, 135, 175, 215, 255}.
  auto cube_index = [](int v) {
    return v < 48 ? 0 : v < 116 ? 1 : (v - 36) / 40;
  };
  auto cube_level = [](int i) { return i == 0 ? 0 : 55 + 40 * i; };
  const int ri = cube_index(r);
  const int gi = cube_index(g);
  const int bi = cube_index(b);
  const int cr = cube_level(ri) - r;
  const int cg = cube_level(gi) - g;
  const int cb = cube_level(bi) - b;
  const int cube_distance = cr * cr + cg * cg + cb * cb;

  // Colors [232, 255] are the grays {8, 18, ..., 238}. The closest one is the
  // closest to the mean (r + g + b) / 3.
  const int gray_index = std::clamp((r + g + b - 10) / 30, 0, 23);
  const int gray_level = 8 + 10 * gray_index;
  const int dr = gray_level - r;
  const int dg = gray_level - g;
  const int db = gray_level - b;
  const int gray_distance = dr * dr + dg * dg + db * db;

  return gray_distance < cube_distance ? 232 + gray_index
                                       : 16 + 36 * ri + 6 * gi + bi;
}

// Print the color the terminal supports closest to either the RGB color
// (r, g, b), or the Palette256 color r. Kept out of line, so that printing
// supported colors stays fast.
FTXUI_NOINLINE void PrintDegraded(std::string& out,
                                  bool is_background_color,
                                  bool is_rgb,
                                  uint8_t r,
                                  uint8_t g,
                                  uint8_t b,
                                  Terminal::Color support) {
  if (support == Terminal::Color::Palette1) {
    out.append(is_background_color ? "49" : "39", 2);
    return;
  }
  const uint8_t index = is_rgb ? ClosestPalette256(r, g, b) : r;
  if (support == Terminal::Color::Palette256) {
    out.append(is_background_color ? "48;5;" : "38;5;", 5);
    AppendNumber(out, index);
    return;
  }
  const uint8_t index_16 = GetColorInfo(Color::Palette256(index)).index_16;
  out.append(palette16code[2 * index_16 + (is_background_color ? 1 : 0)]);
}

}  // namespace

bool Color::operator==(const Color& rhs) const {
  return red_ == rhs.red_ && green_ == rhs.green_ && blue_ == rhs.blue_ &&
         type_ == rhs.type_;
}

bool Color::operator!=(const Color& rhs) const {
  return !operator==(rhs);
}

std::string Color::Print(bool is_background_color) const {
  std::string out;
  PrintTo(out, is_background_color);
  return out;
}

/// @brief Append the ANSI color code to a string (zero-allocation fast path).
/// @param out The string to append to.
/// @param is_background_color Whether this is a background color code.
void Color::PrintTo(std::string& out, bool is_background_color) const {
  PrintTo(out, is_background_color, Terminal::ColorSupport());
}

/// @brief Append the ANSI color code to a string, degraded to the given
/// terminal color support.
/// @param out The string to append to.
/// @param is_background_color Whether this is a background color code.
/// @param support The terminal color support.
void Color::PrintTo(std::string& out,
                    bool is_background_color,
                    Terminal::Color support) const {
  // Degrade the color to what the terminal supports. Both enums are ordered
  // the same way.
  if (static_cast<uint8_t>(type_) > static_cast<uint8_t>(support)) {
    PrintDegraded(out, is_background_color, type_ == ColorType::TrueColor, red_,
                  green_, blue_, support);
    return;
  }

  switch (type_) {
    case ColorType::Palette1:
      out.append(is_background_color ? "49" : "39", 2);
      return;
    case ColorType::Palette16:
      out.append(palette16code[2 * red_ + (is_background_color ? 1 : 0)]);
      return;
    case ColorType::Palette256:
      out.append(is_background_color ? "48;5;" : "38;5;", 5);
      AppendNumber(out, red_);
      return;
    case ColorType::TrueColor:
      out.append(is_background_color ? "48;2;" : "38;2;", 5);
      AppendNumber(out, red_);
      out += ';';
      AppendNumber(out, green_);
      out += ';';
      AppendNumber(out, blue_);
      return;
  }
}

/// @brief Build a transparent color.
Color::Color() = default;

/// @brief Build a transparent color.
Color::Color(Palette1 /*value*/) : Color() {}

/// @brief Build a color using the Palette16 colors.
Color::Color(Palette16 index)
    : type_(ColorType::Palette16), red_(index), alpha_(255) {}

/// @brief Build a color using Palette256 colors.
Color::Color(Palette256 index)
    : type_(ColorType::Palette256), red_(index), alpha_(255) {}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @param alpha The quantity of alpha [0,255]
Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    : type_(ColorType::TrueColor),
      red_(red),
      green_(green),
      blue_(blue),
      alpha_(alpha) {}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
// static
Color Color::RGB(uint8_t red, uint8_t green, uint8_t blue) {
  return RGBA(red, green, blue, 255);
}

/// @brief Build a Color from its RGBA representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @param alpha The quantity of alpha [0,255]
/// @see Color::RGB
// static
Color Color::RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  return {red, green, blue, alpha};
}

/// @brief Build a Color from its HSV representation.
/// https://en.wikipedia.org/wiki/HSL_and_HSV
///
/// @param h The hue of the color [0,255]
/// @param s The "colorfulness" [0,255].
/// @param v The "Lightness" [0,255]
/// @param alpha The quantity of alpha [0,255]
// static
Color Color::HSVA(uint8_t h, uint8_t s, uint8_t v, uint8_t alpha) {
  uint8_t region = h / 43;                                        // NOLINT
  uint8_t remainder = (h - (region * 43)) * 6;                    // NOLINT
  uint8_t p = (v * (255 - s)) >> 8;                               // NOLINT
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;          // NOLINT
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;  // NOLINT

  // clang-format off
  switch (region) {                     // NOLINT
    case 0: return Color(v,t,p, alpha); // NOLINT
    case 1: return Color(q,v,p, alpha); // NOLINT
    case 2: return Color(p,v,t, alpha); // NOLINT
    case 3: return Color(p,q,v, alpha); // NOLINT
    case 4: return Color(t,p,v, alpha); // NOLINT
    case 5: return Color(v,p,q, alpha); // NOLINT
  }                                     // NOLINT
  // clang-format on
  return {0, 0, 0, alpha};
}

/// @brief Build a Color from its HSV representation.
/// https://en.wikipedia.org/wiki/HSL_and_HSV
///
/// @param h The hue of the color [0,255]
/// @param s The "colorfulness" [0,255].
/// @param v The "Lightness" [0,255]
// static
Color Color::HSV(uint8_t h, uint8_t s, uint8_t v) {
  return HSVA(h, s, v, 255);
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

  // Gamma correction:
  // https://en.wikipedia.org/wiki/Gamma_correction
  auto interp = [t](uint8_t a_u, uint8_t b_u) {
    constexpr float gamma = 2.2F;
    const float a_f = std::pow(a_u, gamma);
    const float b_f = std::pow(b_u, gamma);
    const float c_f = a_f * (1.0F - t) +  //
                      b_f * t;
    return static_cast<uint8_t>(std::pow(c_f, 1.F / gamma));
  };
  return Color::RGB(interp(a.GetRed(), b.GetRed()),      //
                    interp(a.GetGreen(), b.GetGreen()),  //
                    interp(a.GetBlue(), b.GetBlue()));   //
}

/// @brief The red component of the color [0,255].
/// Palette colors are resolved to their RGB approximation. Returns 0 for the
/// default (transparent) color.
uint8_t Color::GetRed() const {
  switch (type_) {
    case ColorType::Palette1:
      return 0;
    case ColorType::Palette16:
    case ColorType::Palette256:
      return GetColorInfo(Palette256(red_)).red;
    case ColorType::TrueColor:
      return red_;
  }
  return 0;
}

/// @brief The green component of the color [0,255].
/// Palette colors are resolved to their RGB approximation. Returns 0 for the
/// default (transparent) color.
uint8_t Color::GetGreen() const {
  switch (type_) {
    case ColorType::Palette1:
      return 0;
    case ColorType::Palette16:
    case ColorType::Palette256:
      return GetColorInfo(Palette256(red_)).green;
    case ColorType::TrueColor:
      return green_;
  }
  return 0;
}

/// @brief The blue component of the color [0,255].
/// Palette colors are resolved to their RGB approximation. Returns 0 for the
/// default (transparent) color.
uint8_t Color::GetBlue() const {
  switch (type_) {
    case ColorType::Palette1:
      return 0;
    case ColorType::Palette16:
    case ColorType::Palette256:
      return GetColorInfo(Palette256(red_)).blue;
    case ColorType::TrueColor:
      return blue_;
  }
  return 0;
}

/// @brief The alpha component of the color [0,255]. 0 is fully transparent.
uint8_t Color::GetAlpha() const {
  return alpha_;
}

/// @brief Blend two colors together using the alpha channel.
// static
Color Color::Blend(const Color& lhs, const Color& rhs) {
  Color out = Interpolate(float(rhs.alpha_) / 255.F, lhs, rhs);
  out.alpha_ = lhs.alpha_ + rhs.alpha_ - lhs.alpha_ * rhs.alpha_ / 255;
  return out;
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
