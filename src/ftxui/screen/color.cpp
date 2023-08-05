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
  return color.all == rhs.color.all;
}

bool Color::operator!=(const Color& rhs) const {
  return !operator==(rhs);
}

std::string nbrToString[] = {
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "10",
  "11",
  "12",
  "13",
  "14",
  "15",
  "16",
  "17",
  "18",
  "19",
  "20",
  "21",
  "22",
  "23",
  "24",
  "25",
  "26",
  "27",
  "28",
  "29",
  "30",
  "31",
  "32",
  "33",
  "34",
  "35",
  "36",
  "37",
  "38",
  "39",
  "40",
  "41",
  "42",
  "43",
  "44",
  "45",
  "46",
  "47",
  "48",
  "49",
  "50",
  "51",
  "52",
  "53",
  "54",
  "55",
  "56",
  "57",
  "58",
  "59",
  "60",
  "61",
  "62",
  "63",
  "64",
  "65",
  "66",
  "67",
  "68",
  "69",
  "70",
  "71",
  "72",
  "73",
  "74",
  "75",
  "76",
  "77",
  "78",
  "79",
  "80",
  "81",
  "82",
  "83",
  "84",
  "85",
  "86",
  "87",
  "88",
  "89",
  "90",
  "91",
  "92",
  "93",
  "94",
  "95",
  "96",
  "97",
  "98",
  "99",
  "100",
  "101",
  "102",
  "103",
  "104",
  "105",
  "106",
  "107",
  "108",
  "109",
  "110",
  "111",
  "112",
  "113",
  "114",
  "115",
  "116",
  "117",
  "118",
  "119",
  "120",
  "121",
  "122",
  "123",
  "124",
  "125",
  "126",
  "127",
  "128",
  "129",
  "130",
  "131",
  "132",
  "133",
  "134",
  "135",
  "136",
  "137",
  "138",
  "139",
  "140",
  "141",
  "142",
  "143",
  "144",
  "145",
  "146",
  "147",
  "148",
  "149",
  "150",
  "151",
  "152",
  "153",
  "154",
  "155",
  "156",
  "157",
  "158",
  "159",
  "160",
  "161",
  "162",
  "163",
  "164",
  "165",
  "166",
  "167",
  "168",
  "169",
  "170",
  "171",
  "172",
  "173",
  "174",
  "175",
  "176",
  "177",
  "178",
  "179",
  "180",
  "181",
  "182",
  "183",
  "184",
  "185",
  "186",
  "187",
  "188",
  "189",
  "190",
  "191",
  "192",
  "193",
  "194",
  "195",
  "196",
  "197",
  "198",
  "199",
  "200",
  "201",
  "202",
  "203",
  "204",
  "205",
  "206",
  "207",
  "208",
  "209",
  "210",
  "211",
  "212",
  "213",
  "214",
  "215",
  "216",
  "217",
  "218",
  "219",
  "220",
  "221",
  "222",
  "223",
  "224",
  "225",
  "226",
  "227",
  "228",
  "229",
  "230",
  "231",
  "232",
  "233",
  "234",
  "235",
  "236",
  "237",
  "238",
  "239",
  "240",
  "241",
  "242",
  "243",
  "244",
  "245",
  "246",
  "247",
  "248",
  "249",
  "250",
  "251",
  "252",
  "253",
  "254",
  "255"
};

std::string Color::Print(bool is_background_color) const {
  switch (color.channel.type_) {
    case ColorType::ColorPalette1:
      return is_background_color ? "49"s : "39"s;

    case ColorType::ColorPalette16:
      return palette16code[2 * color.channel.red_ + is_background_color];  // NOLINT;

    case ColorType::ColorPalette256:
      return (is_background_color ? "48;5;"s : "38;5;"s) + nbrToString[color.channel.red_];

    case ColorType::TrueColor:
    default:
      return (is_background_color ? "48;2;"s : "38;2;"s)  //
             + nbrToString[color.channel.red_] + ";"                 //
             + nbrToString[color.channel.green_] + ";"               //
             + nbrToString[color.channel.blue_];                     //
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
Color::Color(Palette16 index) : color(index) {
}

/// @brief Build a transparent using Palette256 colors.
/// @ingroup screen
Color::Color(Palette256 index) : color(index) {
  if (Terminal::ColorSupport() >= Terminal::Color::Palette256) {
    return;
  }
  color.channel.red_ = GetColorInfo(Color::Palette256(color.channel.red_)).index_16;
}

/// @brief Build a Color from its RGB representation.
/// https://en.wikipedia.org/wiki/RGB_color_model
///
/// @param red The quantity of red [0,255]
/// @param green The quantity of green [0,255]
/// @param blue The quantity of blue [0,255]
/// @ingroup screen
Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : color(red, green, blue) {
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
    color.channel.type_ = ColorType::ColorPalette256;
    color.channel.red_ = best;
  } else {
    color.channel.type_ = ColorType::ColorPalette16;
    color.channel.red_ = GetColorInfo(Color::Palette256(best)).index_16;
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
  if (a.color.channel.type_ == ColorType::ColorPalette1 ||  //
      b.color.channel.type_ == ColorType::ColorPalette1) {
    if (t < 0.5F) {  // NOLINT
      return a;
    } else {
      return b;
    }
  }

  auto get_color = [](const Color& color,  //
                      uint8_t* red, uint8_t* green, uint8_t* blue) {
    switch (color.color.channel.type_) {
      case ColorType::ColorPalette1: {
        return;
      }

      case ColorType::ColorPalette16: {
        const ColorInfo info = GetColorInfo(Color::Palette16(color.color.channel.red_));
        *red = info.red;
        *green = info.green;
        *blue = info.blue;
        return;
      }

      case ColorType::ColorPalette256: {
        const ColorInfo info = GetColorInfo(Color::Palette256(color.color.channel.red_));
        *red = info.red;
        *green = info.green;
        *blue = info.blue;
        return;
      }

      case ColorType::TrueColor:
      default: {
        *red = color.color.channel.red_;
        *green = color.color.channel.green_;
        *blue = color.color.channel.blue_;
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
  constexpr float gamma = 2.2f;
  return Color::RGB(
      uint8_t(pow(pow(a_r, gamma) * (1 - t) + pow(b_r, gamma) * t, 1 / gamma)),
      uint8_t(pow(pow(a_g, gamma) * (1 - t) + pow(b_g, gamma) * t, 1 / gamma)),
      uint8_t(pow(pow(a_b, gamma) * (1 - t) + pow(b_b, gamma) * t, 1 / gamma)));
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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
