// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_COLOR_HPP
#define FTXUI_SCREEN_COLOR_HPP

#include <cstdint>  // for uint8_t
#include <string>   // for string
#include <vector>   // for vector

#ifdef RGB
// Workaround for wingdi.h (via Windows.h) defining macros that break things.
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-rgb
#undef RGB
#endif

namespace ftxui {

/// @brief A class representing terminal colors.
/// @ingroup screen
class Color {
 public:
  enum Palette1 : uint8_t;
  enum Palette16 : uint8_t;
  enum Palette256 : uint8_t;

  Color();                  // Transparent.
  Color(Palette1 index);    // Transparent.
  Color(Palette16 index);   // Implicit conversion from index to Color.
  Color(Palette256 index);  // Implicit conversion from index to Color.
  Color(uint8_t red, uint8_t green, uint8_t blue);
  static Color RGB(uint8_t red, uint8_t green, uint8_t blue);
  static Color HSV(uint8_t hue, uint8_t saturation, uint8_t value);
  static Color Interpolate(float t, const Color& a, const Color& b);

  //---------------------------
  // List of colors:
  //---------------------------
  // clang-format off
  enum Palette1 : uint8_t{
    Default, // Transparent
  };

  enum Palette16 : uint8_t {
    Black        = 0,
    Red          = 1,
    Green        = 2,
    Yellow       = 3,
    Blue         = 4,
    Magenta      = 5,
    Cyan         = 6,
    GrayLight    = 7,
    GrayDark     = 8,
    RedLight     = 9,
    GreenLight   = 10,
    YellowLight  = 11,
    BlueLight    = 12,
    MagentaLight = 13,
    CyanLight    = 14,
    White        = 15,
  };

  enum Palette256 : uint8_t {
    Aquamarine1        = 122,
    Aquamarine1Bis     = 86,
    Aquamarine3        = 79,
    Blue1              = 21,
    Blue3              = 19,
    Blue3Bis           = 20,
    BlueViolet         = 57,
    CadetBlue          = 72,
    CadetBlueBis       = 73,
    Chartreuse1        = 118,
    Chartreuse2        = 112,
    Chartreuse2Bis     = 82,
    Chartreuse3        = 70,
    Chartreuse3Bis     = 76,
    Chartreuse4        = 64,
    CornflowerBlue     = 69,
    Cornsilk1          = 230,
    Cyan1              = 51,
    Cyan2              = 50,
    Cyan3              = 43,
    DarkBlue           = 18,
    DarkCyan           = 36,
    DarkGoldenrod      = 136,
    DarkGreen          = 22,
    DarkKhaki          = 143,
    DarkMagenta        = 90,
    DarkMagentaBis     = 91,
    DarkOliveGreen1    = 191,
    DarkOliveGreen1Bis = 192,
    DarkOliveGreen2    = 155,
    DarkOliveGreen3    = 107,
    DarkOliveGreen3Bis = 113,
    DarkOliveGreen3Ter = 149,
    DarkOrange         = 208,
    DarkOrange3        = 130,
    DarkOrange3Bis     = 166,
    DarkRed            = 52,
    DarkRedBis         = 88,
    DarkSeaGreen       = 108,
    DarkSeaGreen1      = 158,
    DarkSeaGreen1Bis   = 193,
    DarkSeaGreen2      = 151,
    DarkSeaGreen2Bis   = 157,
    DarkSeaGreen3      = 115,
    DarkSeaGreen3Bis   = 150,
    DarkSeaGreen4      = 65,
    DarkSeaGreen4Bis   = 71,
    DarkSlateGray1     = 123,
    DarkSlateGray2     = 87,
    DarkSlateGray3     = 116,
    DarkTurquoise      = 44,
    DarkViolet         = 128,
    DarkVioletBis      = 92,
    DeepPink1          = 198,
    DeepPink1Bis       = 199,
    DeepPink2          = 197,
    DeepPink3          = 161,
    DeepPink3Bis       = 162,
    DeepPink4          = 125,
    DeepPink4Bis          = 89,
    DeepPink4Ter       = 53,
    DeepSkyBlue1       = 39,
    DeepSkyBlue2       = 38,
    DeepSkyBlue3       = 31,
    DeepSkyBlue3Bis    = 32,
    DeepSkyBlue4       = 23,
    DeepSkyBlue4Bis    = 24,
    DeepSkyBlue4Ter    = 25,
    DodgerBlue1        = 33,
    DodgerBlue2        = 27,
    DodgerBlue3        = 26,
    Gold1              = 220,
    Gold3              = 142,
    Gold3Bis           = 178,
    Green1             = 46,
    Green3             = 34,
    Green3Bis          = 40,
    Green4             = 28,
    GreenYellow        = 154,
    Grey0              = 16,
    Grey100            = 231,
    Grey11             = 234,
    Grey15             = 235,
    Grey19             = 236,
    Grey23             = 237,
    Grey27             = 238,
    Grey3              = 232,
    Grey30             = 239,
    Grey35             = 240,
    Grey37             = 59,
    Grey39             = 241,
    Grey42             = 242,
    Grey46             = 243,
    Grey50             = 244,
    Grey53             = 102,
    Grey54             = 245,
    Grey58             = 246,
    Grey62             = 247,
    Grey63             = 139,
    Grey66             = 248,
    Grey69             = 145,
    Grey7              = 233,
    Grey70             = 249,
    Grey74             = 250,
    Grey78             = 251,
    Grey82             = 252,
    Grey84             = 188,
    Grey85             = 253,
    Grey89             = 254,
    Grey93             = 255,
    Honeydew2          = 194,
    HotPink            = 205,
    HotPink2           = 169,
    HotPink3           = 132,
    HotPink3Bis        = 168,
    HotPinkBis         = 206,
    IndianRed          = 131,
    IndianRed1         = 203,
    IndianRed1Bis      = 204,
    IndianRedBis       = 167,
    Khaki1             = 228,
    Khaki3             = 185,
    LightCoral         = 210,
    LightCyan1Bis      = 195,
    LightCyan3         = 152,
    LightGoldenrod1    = 227,
    LightGoldenrod2    = 186,
    LightGoldenrod2Bis = 221,
    LightGoldenrod2Ter = 222,
    LightGoldenrod3    = 179,
    LightGreen         = 119,
    LightGreenBis      = 120,
    LightPink1         = 217,
    LightPink3         = 174,
    LightPink4         = 95,
    LightSalmon1       = 216,
    LightSalmon3       = 137,
    LightSalmon3Bis       = 173,
    LightSeaGreen      = 37,
    LightSkyBlue1      = 153,
    LightSkyBlue3      = 109,
    LightSkyBlue3Bis      = 110,
    LightSlateBlue     = 105,
    LightSlateGrey     = 103,
    LightSteelBlue     = 147,
    LightSteelBlue1    = 189,
    LightSteelBlue3    = 146,
    LightYellow3       = 187,
    Magenta1           = 201,
    Magenta2           = 165,
    Magenta2Bis        = 200,
    Magenta3           = 127,
    Magenta3Bis        = 163,
    Magenta3Ter        = 164,
    MediumOrchid       = 134,
    MediumOrchid1      = 171,
    MediumOrchid1Bis   = 207,
    MediumOrchid3      = 133,
    MediumPurple       = 104,
    MediumPurple1      = 141,
    MediumPurple2      = 135,
    MediumPurple2Bis   = 140,
    MediumPurple3      = 97,
    MediumPurple3Bis   = 98,
    MediumPurple4      = 60,
    MediumSpringGreen  = 49,
    MediumTurquoise    = 80,
    MediumVioletRed    = 126,
    MistyRose1         = 224,
    MistyRose3         = 181,
    NavajoWhite1       = 223,
    NavajoWhite3       = 144,
    NavyBlue           = 17,
    Orange1            = 214,
    Orange3            = 172,
    Orange4            = 58,
    Orange4Bis         = 94,
    OrangeRed1         = 202,
    Orchid             = 170,
    Orchid1            = 213,
    Orchid2            = 212,
    PaleGreen1         = 121,
    PaleGreen1Bis      = 156,
    PaleGreen3         = 114,
    PaleGreen3Bis      = 77,
    PaleTurquoise1     = 159,
    PaleTurquoise4     = 66,
    PaleVioletRed1     = 211,
    Pink1              = 218,
    Pink3              = 175,
    Plum1              = 219,
    Plum2              = 183,
    Plum3              = 176,
    Plum4              = 96,
    Purple             = 129,
    Purple3            = 56,
    Purple4            = 54,
    Purple4Bis         = 55,
    PurpleBis          = 93,
    Red1               = 196,
    Red3               = 124,
    Red3Bis            = 160,
    RosyBrown          = 138,
    RoyalBlue1         = 63,
    Salmon1            = 209,
    SandyBrown         = 215,
    SeaGreen1          = 84,
    SeaGreen1Bis       = 85,
    SeaGreen2          = 83,
    SeaGreen3          = 78,
    SkyBlue1           = 117,
    SkyBlue2           = 111,
    SkyBlue3           = 74,
    SlateBlue1         = 99,
    SlateBlue3         = 61,
    SlateBlue3Bis      = 62,
    SpringGreen1       = 48,
    SpringGreen2       = 42,
    SpringGreen2Bis    = 47,
    SpringGreen3       = 35,
    SpringGreen3Bis    = 41,
    SpringGreen4       = 29,
    SteelBlue          = 67,
    SteelBlue1         = 75,
    SteelBlue1Bis      = 81,
    SteelBlue3         = 68,
    Tan                = 180,
    Thistle1           = 225,
    Thistle3           = 182,
    Turquoise2         = 45,
    Turquoise4         = 30,
    Violet             = 177,
    Wheat1             = 229,
    Wheat4             = 101,
    Yellow1            = 226,
    Yellow2            = 190,
    Yellow3            = 148,
    Yellow3Bis         = 184,
    Yellow4            = 100,
    Yellow4Bis         = 106,
  };
  // clang-format on

  // --- Operators ------
  bool operator==(const Color& rhs) const;
  bool operator!=(const Color& rhs) const;

  std::string Print(bool is_background_color) const;

 private:
  enum class ColorType : uint8_t {
    Palette1,
    Palette16,
    Palette256,
    TrueColor,
  };
  ColorType type_ = ColorType::Palette1;
  uint8_t red_ = 0;
  uint8_t green_ = 0;
  uint8_t blue_ = 0;
};

inline namespace literals {

/// @brief Creates a color from a combined hex RGB representation,
/// e.g. 0x808000_rgb
Color operator""_rgb(unsigned long long int combined);

}  // namespace literals

}  // namespace ftxui

#endif  // FTXUI_SCREEN_COLOR_HPP
