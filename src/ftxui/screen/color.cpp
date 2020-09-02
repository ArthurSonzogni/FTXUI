#include "ftxui/screen/color.hpp"

#include <algorithm>

#include "ftxui/screen/string.hpp"

namespace ftxui {

struct Color256Info {
  const char* name;
  const uint8_t index;
  const uint8_t red;
  const uint8_t green;
  const uint8_t blue;
};

// clang-format off
const Color256Info color256_info[] = {
     {"Grey0"              , 16  , 0   , 0   , 0   } ,
     {"NavyBlue"           , 17  , 0   , 0   , 95  } ,
     {"DarkBlue"           , 18  , 0   , 0   , 135 } ,
     {"Blue3"              , 19  , 0   , 0   , 175 } ,
     {"Blue3Bis"           , 20  , 0   , 0   , 215 } ,
     {"Blue1"              , 21  , 0   , 0   , 255 } ,
     {"DarkGreen"          , 22  , 0   , 95  , 0   } ,
     {"DeepSkyBlue4"       , 23  , 0   , 95  , 95  } ,
     {"DeepSkyBlue4Bis"    , 24  , 0   , 95  , 135 } ,
     {"DeepSkyBlue4Ter"    , 25  , 0   , 95  , 175 } ,
     {"DodgerBlue3"        , 26  , 0   , 95  , 215 } ,
     {"DodgerBlue2"        , 27  , 0   , 95  , 255 } ,
     {"Green4"             , 28  , 0   , 135 , 0   } ,
     {"SpringGreen4"       , 29  , 0   , 135 , 95  } ,
     {"Turquoise4"         , 30  , 0   , 135 , 135 } ,
     {"DeepSkyBlue3"       , 31  , 0   , 135 , 175 } ,
     {"DeepSkyBlue3Bis"    , 32  , 0   , 135 , 215 } ,
     {"DodgerBlue1"        , 33  , 0   , 135 , 255 } ,
     {"Green3"             , 34  , 0   , 175 , 0   } ,
     {"SpringGreen3"       , 35  , 0   , 175 , 95  } ,
     {"DarkCyan"           , 36  , 0   , 175 , 135 } ,
     {"LightSeaGreen"      , 37  , 0   , 175 , 175 } ,
     {"DeepSkyBlue2"       , 38  , 0   , 175 , 215 } ,
     {"DeepSkyBlue1"       , 39  , 0   , 175 , 255 } ,
     {"Green3Bis"          , 40  , 0   , 215 , 0   } ,
     {"SpringGreen3Bis"    , 41  , 0   , 215 , 95  } ,
     {"SpringGreen2"       , 42  , 0   , 215 , 135 } ,
     {"Cyan3"              , 43  , 0   , 215 , 175 } ,
     {"DarkTurquoise"      , 44  , 0   , 215 , 215 } ,
     {"Turquoise2"         , 45  , 0   , 215 , 255 } ,
     {"Green1"             , 46  , 0   , 255 , 0   } ,
     {"SpringGreen2Bis"    , 47  , 0   , 255 , 95  } ,
     {"SpringGreen1"       , 48  , 0   , 255 , 135 } ,
     {"MediumSpringGreen"  , 49  , 0   , 255 , 175 } ,
     {"Cyan2"              , 50  , 0   , 255 , 215 } ,
     {"Cyan1"              , 51  , 0   , 255 , 255 } ,
     {"DarkRed"            , 52  , 95  , 0   , 0   } ,
     {"DeepPink4Ter"       , 53  , 95  , 0   , 95  } ,
     {"Purple4"            , 54  , 95  , 0   , 135 } ,
     {"Purple4Bis"         , 55  , 95  , 0   , 175 } ,
     {"Purple3"            , 56  , 95  , 0   , 215 } ,
     {"BlueViolet"         , 57  , 95  , 0   , 255 } ,
     {"Orange4"            , 58  , 95  , 95  , 0   } ,
     {"Grey37"             , 59  , 95  , 95  , 95  } ,
     {"MediumPurple4"      , 60  , 95  , 95  , 135 } ,
     {"SlateBlue3"         , 61  , 95  , 95  , 175 } ,
     {"SlateBlue3Bis"      , 62  , 95  , 95  , 215 } ,
     {"RoyalBlue1"         , 63  , 95  , 95  , 255 } ,
     {"Chartreuse4"        , 64  , 95  , 135 , 0   } ,
     {"DarkSeaGreen4"      , 65  , 95  , 135 , 95  } ,
     {"PaleTurquoise4"     , 66  , 95  , 135 , 135 } ,
     {"SteelBlue"          , 67  , 95  , 135 , 175 } ,
     {"SteelBlue3"         , 68  , 95  , 135 , 215 } ,
     {"CornflowerBlue"     , 69  , 95  , 135 , 255 } ,
     {"Chartreuse3"        , 70  , 95  , 175 , 0   } ,
     {"DarkSeaGreen4Bis"   , 71  , 95  , 175 , 95  } ,
     {"CadetBlue"          , 72  , 95  , 175 , 135 } ,
     {"CadetBlueBis"       , 73  , 95  , 175 , 175 } ,
     {"SkyBlue3"           , 74  , 95  , 175 , 215 } ,
     {"SteelBlue1"         , 75  , 95  , 175 , 255 } ,
     {"Chartreuse3Bis"     , 76  , 95  , 215 , 0   } ,
     {"PaleGreen3Bis"      , 77  , 95  , 215 , 95  } ,
     {"SeaGreen3"          , 78  , 95  , 215 , 135 } ,
     {"Aquamarine3"        , 79  , 95  , 215 , 175 } ,
     {"MediumTurquoise"    , 80  , 95  , 215 , 215 } ,
     {"SteelBlue1Bis"      , 81  , 95  , 215 , 255 } ,
     {"Chartreuse2Bis"     , 82  , 95  , 255 , 0   } ,
     {"SeaGreen2"          , 83  , 95  , 255 , 95  } ,
     {"SeaGreen1"          , 84  , 95  , 255 , 135 } ,
     {"SeaGreen1Bis"       , 85  , 95  , 255 , 175 } ,
     {"Aquamarine1Bis"     , 86  , 95  , 255 , 215 } ,
     {"DarkSlateGray2"     , 87  , 95  , 255 , 255 } ,
     {"DarkRedBis"         , 88  , 135 , 0   , 0   } ,
     {"DeepPink4Bis"       , 89  , 135 , 0   , 95  } ,
     {"DarkMagenta"        , 90  , 135 , 0   , 135 } ,
     {"DarkMagentaBis"     , 91  , 135 , 0   , 175 } ,
     {"DarkVioletBis"      , 92  , 135 , 0   , 215 } ,
     {"PurpleBis"          , 93  , 135 , 0   , 255 } ,
     {"Orange4Bis"         , 94  , 135 , 95  , 0   } ,
     {"LightPink4"         , 95  , 135 , 95  , 95  } ,
     {"Plum4"              , 96  , 135 , 95  , 135 } ,
     {"MediumPurple3"      , 97  , 135 , 95  , 175 } ,
     {"MediumPurple3Bis"   , 98  , 135 , 95  , 215 } ,
     {"SlateBlue1"         , 99  , 135 , 95  , 255 } ,
     {"Yellow4"            , 100 , 135 , 135 , 0   } ,
     {"Wheat4"             , 101 , 135 , 135 , 95  } ,
     {"Grey53"             , 102 , 135 , 135 , 135 } ,
     {"LightSlateGrey"     , 103 , 135 , 135 , 175 } ,
     {"MediumPurple"       , 104 , 135 , 135 , 215 } ,
     {"LightSlateBlue"     , 105 , 135 , 135 , 255 } ,
     {"Yellow4Bis"         , 106 , 135 , 175 , 0   } ,
     {"DarkOliveGreen3"    , 107 , 135 , 175 , 95  } ,
     {"DarkSeaGreen"       , 108 , 135 , 175 , 135 } ,
     {"LightSkyBlue3"      , 109 , 135 , 175 , 175 } ,
     {"LightSkyBlue3Bis"   , 110 , 135 , 175 , 215 } ,
     {"SkyBlue2"           , 111 , 135 , 175 , 255 } ,
     {"Chartreuse2"        , 112 , 135 , 215 , 0   } ,
     {"DarkOliveGreen3Bis" , 113 , 135 , 215 , 95  } ,
     {"PaleGreen3"         , 114 , 135 , 215 , 135 } ,
     {"DarkSeaGreen3"      , 115 , 135 , 215 , 175 } ,
     {"DarkSlateGray3"     , 116 , 135 , 215 , 215 } ,
     {"SkyBlue1"           , 117 , 135 , 215 , 255 } ,
     {"Chartreuse1"        , 118 , 135 , 255 , 0   } ,
     {"LightGreen"         , 119 , 135 , 255 , 95  } ,
     {"LightGreenBis"      , 120 , 135 , 255 , 135 } ,
     {"PaleGreen1"         , 121 , 135 , 255 , 175 } ,
     {"Aquamarine1"        , 122 , 135 , 255 , 215 } ,
     {"DarkSlateGray1"     , 123 , 135 , 255 , 255 } ,
     {"Red3"               , 124 , 175 , 0   , 0   } ,
     {"DeepPink4"          , 125 , 175 , 0   , 95  } ,
     {"MediumVioletRed"    , 126 , 175 , 0   , 135 } ,
     {"Magenta3"           , 127 , 175 , 0   , 175 } ,
     {"DarkViolet"         , 128 , 175 , 0   , 215 } ,
     {"Purple"             , 129 , 175 , 0   , 255 } ,
     {"DarkOrange3"        , 130 , 175 , 95  , 0   } ,
     {"IndianRed"          , 131 , 175 , 95  , 95  } ,
     {"HotPink3"           , 132 , 175 , 95  , 135 } ,
     {"MediumOrchid3"      , 133 , 175 , 95  , 175 } ,
     {"MediumOrchid"       , 134 , 175 , 95  , 215 } ,
     {"MediumPurple2"      , 135 , 175 , 95  , 255 } ,
     {"DarkGoldenrod"      , 136 , 175 , 135 , 0   } ,
     {"LightSalmon3"       , 137 , 175 , 135 , 95  } ,
     {"RosyBrown"          , 138 , 175 , 135 , 135 } ,
     {"Grey63"             , 139 , 175 , 135 , 175 } ,
     {"MediumPurple2Bis"   , 140 , 175 , 135 , 215 } ,
     {"MediumPurple1"      , 141 , 175 , 135 , 255 } ,
     {"Gold3"              , 142 , 175 , 175 , 0   } ,
     {"DarkKhaki"          , 143 , 175 , 175 , 95  } ,
     {"NavajoWhite3"       , 144 , 175 , 175 , 135 } ,
     {"Grey69"             , 145 , 175 , 175 , 175 } ,
     {"LightSteelBlue3"    , 146 , 175 , 175 , 215 } ,
     {"LightSteelBlue"     , 147 , 175 , 175 , 255 } ,
     {"Yellow3"            , 148 , 175 , 215 , 0   } ,
     {"DarkOliveGreen3Ter" , 149 , 175 , 215 , 95  } ,
     {"DarkSeaGreen3Bis"   , 150 , 175 , 215 , 135 } ,
     {"DarkSeaGreen2"      , 151 , 175 , 215 , 175 } ,
     {"LightCyan3"         , 152 , 175 , 215 , 215 } ,
     {"LightSkyBlue1"      , 153 , 175 , 215 , 255 } ,
     {"GreenYellow"        , 154 , 175 , 255 , 0   } ,
     {"DarkOliveGreen2"    , 155 , 175 , 255 , 95  } ,
     {"PaleGreen1Bis"      , 156 , 175 , 255 , 135 } ,
     {"DarkSeaGreen2Bis"   , 157 , 175 , 255 , 175 } ,
     {"DarkSeaGreen1"      , 158 , 175 , 255 , 215 } ,
     {"PaleTurquoise1"     , 159 , 175 , 255 , 255 } ,
     {"Red3Bis"            , 160 , 215 , 0   , 0   } ,
     {"DeepPink3"          , 161 , 215 , 0   , 95  } ,
     {"DeepPink3Bis"       , 162 , 215 , 0   , 135 } ,
     {"Magenta3Bis"        , 163 , 215 , 0   , 175 } ,
     {"Magenta3Ter"        , 164 , 215 , 0   , 215 } ,
     {"Magenta2"           , 165 , 215 , 0   , 255 } ,
     {"DarkOrange3Bis"     , 166 , 215 , 95  , 0   } ,
     {"IndianRedBis"       , 167 , 215 , 95  , 95  } ,
     {"HotPink3Bis"        , 168 , 215 , 95  , 135 } ,
     {"HotPink2"           , 169 , 215 , 95  , 175 } ,
     {"Orchid"             , 170 , 215 , 95  , 215 } ,
     {"MediumOrchid1"      , 171 , 215 , 95  , 255 } ,
     {"Orange3"            , 172 , 215 , 135 , 0   } ,
     {"LightSalmon3"       , 173 , 215 , 135 , 95  } ,
     {"LightPink3"         , 174 , 215 , 135 , 135 } ,
     {"Pink3"              , 175 , 215 , 135 , 175 } ,
     {"Plum3"              , 176 , 215 , 135 , 215 } ,
     {"Violet"             , 177 , 215 , 135 , 255 } ,
     {"Gold3Bis"           , 178 , 215 , 175 , 0   } ,
     {"LightGoldenrod3"    , 179 , 215 , 175 , 95  } ,
     {"Tan"                , 180 , 215 , 175 , 135 } ,
     {"MistyRose3"         , 181 , 215 , 175 , 175 } ,
     {"Thistle3"           , 182 , 215 , 175 , 215 } ,
     {"Plum2"              , 183 , 215 , 175 , 255 } ,
     {"Yellow3Bis"         , 184 , 215 , 215 , 0   } ,
     {"Khaki3"             , 185 , 215 , 215 , 95  } ,
     {"LightGoldenrod2"    , 186 , 215 , 215 , 135 } ,
     {"LightYellow3"       , 187 , 215 , 215 , 175 } ,
     {"Grey84"             , 188 , 215 , 215 , 215 } ,
     {"LightSteelBlue1"    , 189 , 215 , 215 , 255 } ,
     {"Yellow2"            , 190 , 215 , 255 , 0   } ,
     {"DarkOliveGreen1"    , 191 , 215 , 255 , 95  } ,
     {"DarkOliveGreen1Bis" , 192 , 215 , 255 , 135 } ,
     {"DarkSeaGreen1Bis"   , 193 , 215 , 255 , 175 } ,
     {"Honeydew2"          , 194 , 215 , 255 , 215 } ,
     {"LightCyan1Bis"      , 195 , 215 , 255 , 255 } ,
     {"Red1"               , 196 , 255 , 0   , 0   } ,
     {"DeepPink2"          , 197 , 255 , 0   , 95  } ,
     {"DeepPink1"          , 198 , 255 , 0   , 135 } ,
     {"DeepPink1Bis"       , 199 , 255 , 0   , 175 } ,
     {"Magenta2Bis"        , 200 , 255 , 0   , 215 } ,
     {"Magenta1"           , 201 , 255 , 0   , 255 } ,
     {"OrangeRed1"         , 202 , 255 , 95  , 0   } ,
     {"IndianRed1"         , 203 , 255 , 95  , 95  } ,
     {"IndianRed1Bis"      , 204 , 255 , 95  , 135 } ,
     {"HotPink"            , 205 , 255 , 95  , 175 } ,
     {"HotPinkBis"         , 206 , 255 , 95  , 215 } ,
     {"MediumOrchid1Bis"   , 207 , 255 , 95  , 255 } ,
     {"DarkOrange"         , 208 , 255 , 135 , 0   } ,
     {"Salmon1"            , 209 , 255 , 135 , 95  } ,
     {"LightCoral"         , 210 , 255 , 135 , 135 } ,
     {"PaleVioletRed1"     , 211 , 255 , 135 , 175 } ,
     {"Orchid2"            , 212 , 255 , 135 , 215 } ,
     {"Orchid1"            , 213 , 255 , 135 , 255 } ,
     {"Orange1"            , 214 , 255 , 175 , 0   } ,
     {"SandyBrown"         , 215 , 255 , 175 , 95  } ,
     {"LightSalmon1"       , 216 , 255 , 175 , 135 } ,
     {"LightPink1"         , 217 , 255 , 175 , 175 } ,
     {"Pink1"              , 218 , 255 , 175 , 215 } ,
     {"Plum1"              , 219 , 255 , 175 , 255 } ,
     {"Gold1"              , 220 , 255 , 215 , 0   } ,
     {"LightGoldenrod2Bis" , 221 , 255 , 215 , 95  } ,
     {"LightGoldenrod2Ter" , 222 , 255 , 215 , 135 } ,
     {"NavajoWhite1"       , 223 , 255 , 215 , 175 } ,
     {"MistyRose1"         , 224 , 255 , 215 , 215 } ,
     {"Thistle1"           , 225 , 255 , 215 , 255 } ,
     {"Yellow1"            , 226 , 255 , 255 , 0   } ,
     {"LightGoldenrod1"    , 227 , 255 , 255 , 95  } ,
     {"Khaki1"             , 228 , 255 , 255 , 135 } ,
     {"Wheat1"             , 229 , 255 , 255 , 175 } ,
     {"Cornsilk1"          , 230 , 255 , 255 , 215 } ,
     {"Grey100"            , 231 , 255 , 255 , 255 } ,
     {"Grey3"              , 232 , 8   , 8   , 8   } ,
     {"Grey7"              , 233 , 18  , 18  , 18  } ,
     {"Grey11"             , 234 , 28  , 28  , 28  } ,
     {"Grey15"             , 235 , 38  , 38  , 38  } ,
     {"Grey19"             , 236 , 48  , 48  , 48  } ,
     {"Grey23"             , 237 , 58  , 58  , 58  } ,
     {"Grey27"             , 238 , 68  , 68  , 68  } ,
     {"Grey30"             , 239 , 78  , 78  , 78  } ,
     {"Grey35"             , 240 , 88  , 88  , 88  } ,
     {"Grey39"             , 241 , 98  , 98  , 98  } ,
     {"Grey42"             , 242 , 108 , 108 , 108 } ,
     {"Grey46"             , 243 , 118 , 118 , 118 } ,
     {"Grey50"             , 244 , 128 , 128 , 128 } ,
     {"Grey54"             , 245 , 138 , 138 , 138 } ,
     {"Grey58"             , 246 , 148 , 148 , 148 } ,
     {"Grey62"             , 247 , 158 , 158 , 158 } ,
     {"Grey66"             , 248 , 168 , 168 , 168 } ,
     {"Grey70"             , 249 , 178 , 178 , 178 } ,
     {"Grey74"             , 250 , 188 , 188 , 188 } ,
     {"Grey78"             , 251 , 198 , 198 , 198 } ,
     {"Grey82"             , 252 , 208 , 208 , 208 } ,
     {"Grey85"             , 253 , 218 , 218 , 218 } ,
     {"Grey89"             , 254 , 228 , 228 , 228 } ,
     {"Grey93"             , 255 , 238 , 238 , 238 } ,
};
// clang-format on

bool Color::operator==(const Color& rhs) const {
  (void)color256_info;
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

Color::Color() : type_(ColorType::Palette16), index_(Palette16::Default) {}
Color::Color(Palette16 index) : type_(ColorType::Palette16), index_(index) {}
Color::Color(Palette256 index) : type_(ColorType::Palette256), index_(index) {}
Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : type_(ColorType::TrueColor), red_(red), green_(green), blue_(blue) {}

// static
Color Color::RGB(uint8_t red, uint8_t green, uint8_t blue) {
  return Color(red, green, blue);
}

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
