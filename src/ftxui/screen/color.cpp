#include "ftxui/screen/color.hpp"

#include "ftxui/screen/string.hpp"

namespace ftxui {

bool Color::operator==(const Color& rhs) const {
  return type_ == rhs.type_ && index_ == rhs.index_ && r_ == rhs.r_ &&
         g_ == rhs.g_ && b_ == rhs.b_;
}

bool Color::operator!=(const Color& rhs) const {
  return !operator==(rhs);
}

std::wstring Color::ToTerminalColorCode(ColorType maximum_color_type_available,
                                        bool is_background_color) const {
  if (maximum_color_type_available < type_) {
    // conversion will be necessary;
  } else {
    switch (type_) {
      case ColorType::None:
        return L"";

      case ColorType::Basic:
        return to_wstring(
            std::to_string((is_background_color ? 10 : 0) + index_));

      case ColorType::Palette256:
        return L"";

      case ColorType::TrueColor:
        return L"";
    }
  }
  return L"";
}

Color Color::Color256(int index) {
  Color color;
  color.type_ = ColorType::Palette256;
  color.index_ = index;
  return color;
}

Color Color::ColorRGB(int r, int g, int b) {
  Color color;
  color.type_ = ColorType::TrueColor;
  color.r_ = r;
  color.g_ = g;
  color.b_ = b;
  return color;
}

Color Color::ColorBasic(int index) {
  Color color;
  color.type_ = ColorType::Basic;
  color.index_ = index;
  return color;
}

Color::Color() {}

// --- Static instances representing the basic color set -----
// --- Transparent -----
Color Color::Default = ColorBasic(39);

// --- Grayscale -----
Color Color::Black = ColorBasic(30);
Color Color::GrayDark = ColorBasic(90);
Color Color::GrayLight = ColorBasic(37);
Color Color::White = ColorBasic(97);

// --- Hue -----
Color Color::Blue = ColorBasic(34);
Color Color::BlueLight = ColorBasic(94);

Color Color::Cyan = ColorBasic(36);
Color Color::CyanLight = ColorBasic(96);

Color Color::Green = ColorBasic(32);
Color Color::GreenLight = ColorBasic(92);

Color Color::Magenta = ColorBasic(35);
Color Color::MagentaLight = ColorBasic(95);

Color Color::Red = ColorBasic(31);
Color Color::RedLight = ColorBasic(91);

Color Color::Yellow = ColorBasic(33);
Color Color::YellowLight = ColorBasic(93);

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
