// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
//
// Content of this file was created thanks to:
// -
// https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/WordBreakProperty.txt
// - Markus Kuhn -- 2007-05-26 (Unicode 5.0)
//   http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
// Thanks you!

#include "ftxui/screen/string.hpp"

#include <array>    // for array
#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t, uint8_t, uint16_t, int32_t
#include <string>   // for string, basic_string, wstring
#include <tuple>    // for _Swallow_assign, ignore
#include <vector>

#include "ftxui/screen/deprecated.hpp"       // for wchar_width, wstring_width
#include "ftxui/screen/string_internal.hpp"  // for WordBreakProperty, EatCodePoint, CodepointToWordBreakProperty, GlyphCount, GlyphIterate, GlyphNext, GlyphPrevious, IsCombining, IsControl, IsFullWidth, Utf8ToWordBreakProperty

namespace {

struct Interval {
  uint32_t first;
  uint32_t last;
};

// As of Unicode 13.0.0
constexpr std::array<Interval, 116> g_full_width_characters = {{
    {0x01100, 0x0115f}, {0x0231a, 0x0231b}, {0x02329, 0x0232a},
    {0x023e9, 0x023ec}, {0x023f0, 0x023f0}, {0x023f3, 0x023f3},
    {0x025fd, 0x025fe}, {0x02614, 0x02615}, {0x02648, 0x02653},
    {0x0267f, 0x0267f}, {0x02693, 0x02693}, {0x026a1, 0x026a1},
    {0x026aa, 0x026ab}, {0x026bd, 0x026be}, {0x026c4, 0x026c5},
    {0x026ce, 0x026ce}, {0x026d4, 0x026d4}, {0x026ea, 0x026ea},
    {0x026f2, 0x026f3}, {0x026f5, 0x026f5}, {0x026fa, 0x026fa},
    {0x026fd, 0x026fd}, {0x02705, 0x02705}, {0x0270a, 0x0270b},
    {0x02728, 0x02728}, {0x0274c, 0x0274c}, {0x0274e, 0x0274e},
    {0x02753, 0x02755}, {0x02757, 0x02757}, {0x02795, 0x02797},
    {0x027b0, 0x027b0}, {0x027bf, 0x027bf}, {0x02b1b, 0x02b1c},
    {0x02b50, 0x02b50}, {0x02b55, 0x02b55}, {0x02e80, 0x02e99},
    {0x02e9b, 0x02ef3}, {0x02f00, 0x02fd5}, {0x02ff0, 0x02ffb},
    {0x03000, 0x0303e}, {0x03041, 0x03096}, {0x03099, 0x030ff},
    {0x03105, 0x0312f}, {0x03131, 0x0318e}, {0x03190, 0x031e3},
    {0x031f0, 0x0321e}, {0x03220, 0x03247}, {0x03250, 0x04dbf},
    {0x04e00, 0x0a48c}, {0x0a490, 0x0a4c6}, {0x0a960, 0x0a97c},
    {0x0ac00, 0x0d7a3}, {0x0f900, 0x0faff}, {0x0fe10, 0x0fe19},
    {0x0fe30, 0x0fe52}, {0x0fe54, 0x0fe66}, {0x0fe68, 0x0fe6b},
    {0x0ff01, 0x0ff60}, {0x0ffe0, 0x0ffe6}, {0x16fe0, 0x16fe4},
    {0x16ff0, 0x16ff1}, {0x17000, 0x187f7}, {0x18800, 0x18cd5},
    {0x18d00, 0x18d08}, {0x1b000, 0x1b11e}, {0x1b150, 0x1b152},
    {0x1b164, 0x1b167}, {0x1b170, 0x1b2fb}, {0x1f004, 0x1f004},
    {0x1f0cf, 0x1f0cf}, {0x1f18e, 0x1f18e}, {0x1f191, 0x1f19a},
    {0x1f200, 0x1f202}, {0x1f210, 0x1f23b}, {0x1f240, 0x1f248},
    {0x1f250, 0x1f251}, {0x1f260, 0x1f265}, {0x1f300, 0x1f320},
    {0x1f32d, 0x1f335}, {0x1f337, 0x1f37c}, {0x1f37e, 0x1f393},
    {0x1f3a0, 0x1f3ca}, {0x1f3cf, 0x1f3d3}, {0x1f3e0, 0x1f3f0},
    {0x1f3f4, 0x1f3f4}, {0x1f3f8, 0x1f43e}, {0x1f440, 0x1f440},
    {0x1f442, 0x1f4fc}, {0x1f4ff, 0x1f53d}, {0x1f54b, 0x1f54e},
    {0x1f550, 0x1f567}, {0x1f57a, 0x1f57a}, {0x1f595, 0x1f596},
    {0x1f5a4, 0x1f5a4}, {0x1f5fb, 0x1f64f}, {0x1f680, 0x1f6c5},
    {0x1f6cc, 0x1f6cc}, {0x1f6d0, 0x1f6d2}, {0x1f6d5, 0x1f6d7},
    {0x1f6eb, 0x1f6ec}, {0x1f6f4, 0x1f6fc}, {0x1f7e0, 0x1f7eb},
    {0x1f90c, 0x1f93a}, {0x1f93c, 0x1f945}, {0x1f947, 0x1f978},
    {0x1f97a, 0x1f9cb}, {0x1f9cd, 0x1f9ff}, {0x1fa70, 0x1fa74},
    {0x1fa78, 0x1fa7a}, {0x1fa80, 0x1fa86}, {0x1fa90, 0x1faa8},
    {0x1fab0, 0x1fab6}, {0x1fac0, 0x1fac2}, {0x1fad0, 0x1fad6},
    {0x20000, 0x2fffd}, {0x30000, 0x3fffd},
}};

using WBP = ftxui::WordBreakProperty;
struct WordBreakPropertyInterval {
  uint32_t first;
  uint32_t last;
  WBP property;
};

// Properties from:
// https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/WordBreakProperty.txt
constexpr std::array<WordBreakPropertyInterval, 993> g_word_break_intervals = {{
    {0x0000A, 0x0000A, WBP::LF},
    {0x0000B, 0x0000C, WBP::Newline},
    {0x0000D, 0x0000D, WBP::CR},
    {0x00020, 0x00020, WBP::WSegSpace},
    {0x00022, 0x00022, WBP::Double_Quote},
    {0x00027, 0x00027, WBP::Single_Quote},
    {0x0002C, 0x0002C, WBP::MidNum},
    {0x0002E, 0x0002E, WBP::MidNumLet},
    {0x00030, 0x00039, WBP::Numeric},
    {0x0003A, 0x0003A, WBP::MidLetter},
    {0x0003B, 0x0003B, WBP::MidNum},
    {0x00041, 0x0005A, WBP::ALetter},
    {0x0005F, 0x0005F, WBP::ExtendNumLet},
    {0x00061, 0x0007A, WBP::ALetter},
    {0x00085, 0x00085, WBP::Newline},
    {0x000AA, 0x000AA, WBP::ALetter},
    {0x000AD, 0x000AD, WBP::Format},
    {0x000B5, 0x000B5, WBP::ALetter},
    {0x000B7, 0x000B7, WBP::MidLetter},
    {0x000BA, 0x000BA, WBP::ALetter},
    {0x000C0, 0x000D6, WBP::ALetter},
    {0x000D8, 0x000F6, WBP::ALetter},
    {0x000F8, 0x002D7, WBP::ALetter},
    {0x002DE, 0x002FF, WBP::ALetter},
    {0x00300, 0x0036F, WBP::Extend},
    {0x00370, 0x00374, WBP::ALetter},
    {0x00376, 0x00377, WBP::ALetter},
    {0x0037A, 0x0037D, WBP::ALetter},
    {0x0037E, 0x0037E, WBP::MidNum},
    {0x0037F, 0x0037F, WBP::ALetter},
    {0x00386, 0x00386, WBP::ALetter},
    {0x00387, 0x00387, WBP::MidLetter},
    {0x00388, 0x0038A, WBP::ALetter},
    {0x0038C, 0x0038C, WBP::ALetter},
    {0x0038E, 0x003A1, WBP::ALetter},
    {0x003A3, 0x003F5, WBP::ALetter},
    {0x003F7, 0x00481, WBP::ALetter},
    {0x00483, 0x00489, WBP::Extend},
    {0x0048A, 0x0052F, WBP::ALetter},
    {0x00531, 0x00556, WBP::ALetter},
    {0x00559, 0x0055C, WBP::ALetter},
    {0x0055E, 0x0055E, WBP::ALetter},
    {0x0055F, 0x0055F, WBP::MidLetter},
    {0x00560, 0x00588, WBP::ALetter},
    {0x00589, 0x00589, WBP::MidNum},
    {0x0058A, 0x0058A, WBP::ALetter},
    {0x00591, 0x005BD, WBP::Extend},
    {0x005BF, 0x005BF, WBP::Extend},
    {0x005C1, 0x005C2, WBP::Extend},
    {0x005C4, 0x005C5, WBP::Extend},
    {0x005C7, 0x005C7, WBP::Extend},
    {0x005D0, 0x005EA, WBP::Hebrew_Letter},
    {0x005EF, 0x005F2, WBP::Hebrew_Letter},
    {0x005F3, 0x005F3, WBP::ALetter},
    {0x005F4, 0x005F4, WBP::MidLetter},
    {0x00600, 0x00605, WBP::Format},
    {0x0060C, 0x0060D, WBP::MidNum},
    {0x00610, 0x0061A, WBP::Extend},
    {0x0061C, 0x0061C, WBP::Format},
    {0x00620, 0x0064A, WBP::ALetter},
    {0x0064B, 0x0065F, WBP::Extend},
    {0x00660, 0x00669, WBP::Numeric},
    {0x0066B, 0x0066B, WBP::Numeric},
    {0x0066C, 0x0066C, WBP::MidNum},
    {0x0066E, 0x0066F, WBP::ALetter},
    {0x00670, 0x00670, WBP::Extend},
    {0x00671, 0x006D3, WBP::ALetter},
    {0x006D5, 0x006D5, WBP::ALetter},
    {0x006D6, 0x006DC, WBP::Extend},
    {0x006DD, 0x006DD, WBP::Format},
    {0x006DF, 0x006E4, WBP::Extend},
    {0x006E5, 0x006E6, WBP::ALetter},
    {0x006E7, 0x006E8, WBP::Extend},
    {0x006EA, 0x006ED, WBP::Extend},
    {0x006EE, 0x006EF, WBP::ALetter},
    {0x006F0, 0x006F9, WBP::Numeric},
    {0x006FA, 0x006FC, WBP::ALetter},
    {0x006FF, 0x006FF, WBP::ALetter},
    {0x0070F, 0x0070F, WBP::Format},
    {0x00710, 0x00710, WBP::ALetter},
    {0x00711, 0x00711, WBP::Extend},
    {0x00712, 0x0072F, WBP::ALetter},
    {0x00730, 0x0074A, WBP::Extend},
    {0x0074D, 0x007A5, WBP::ALetter},
    {0x007A6, 0x007B0, WBP::Extend},
    {0x007B1, 0x007B1, WBP::ALetter},
    {0x007C0, 0x007C9, WBP::Numeric},
    {0x007CA, 0x007EA, WBP::ALetter},
    {0x007EB, 0x007F3, WBP::Extend},
    {0x007F4, 0x007F5, WBP::ALetter},
    {0x007F8, 0x007F8, WBP::MidNum},
    {0x007FA, 0x007FA, WBP::ALetter},
    {0x007FD, 0x007FD, WBP::Extend},
    {0x00800, 0x00815, WBP::ALetter},
    {0x00816, 0x00819, WBP::Extend},
    {0x0081A, 0x0081A, WBP::ALetter},
    {0x0081B, 0x00823, WBP::Extend},
    {0x00824, 0x00824, WBP::ALetter},
    {0x00825, 0x00827, WBP::Extend},
    {0x00828, 0x00828, WBP::ALetter},
    {0x00829, 0x0082D, WBP::Extend},
    {0x00840, 0x00858, WBP::ALetter},
    {0x00859, 0x0085B, WBP::Extend},
    {0x00860, 0x0086A, WBP::ALetter},
    {0x008A0, 0x008B4, WBP::ALetter},
    {0x008B6, 0x008C7, WBP::ALetter},
    {0x008D3, 0x008E1, WBP::Extend},
    {0x008E2, 0x008E2, WBP::Format},
    {0x008E3, 0x00903, WBP::Extend},
    {0x00904, 0x00939, WBP::ALetter},
    {0x0093A, 0x0093C, WBP::Extend},
    {0x0093D, 0x0093D, WBP::ALetter},
    {0x0093E, 0x0094F, WBP::Extend},
    {0x00950, 0x00950, WBP::ALetter},
    {0x00951, 0x00957, WBP::Extend},
    {0x00958, 0x00961, WBP::ALetter},
    {0x00962, 0x00963, WBP::Extend},
    {0x00966, 0x0096F, WBP::Numeric},
    {0x00971, 0x00980, WBP::ALetter},
    {0x00981, 0x00983, WBP::Extend},
    {0x00985, 0x0098C, WBP::ALetter},
    {0x0098F, 0x00990, WBP::ALetter},
    {0x00993, 0x009A8, WBP::ALetter},
    {0x009AA, 0x009B0, WBP::ALetter},
    {0x009B2, 0x009B2, WBP::ALetter},
    {0x009B6, 0x009B9, WBP::ALetter},
    {0x009BC, 0x009BC, WBP::Extend},
    {0x009BD, 0x009BD, WBP::ALetter},
    {0x009BE, 0x009C4, WBP::Extend},
    {0x009C7, 0x009C8, WBP::Extend},
    {0x009CB, 0x009CD, WBP::Extend},
    {0x009CE, 0x009CE, WBP::ALetter},
    {0x009D7, 0x009D7, WBP::Extend},
    {0x009DC, 0x009DD, WBP::ALetter},
    {0x009DF, 0x009E1, WBP::ALetter},
    {0x009E2, 0x009E3, WBP::Extend},
    {0x009E6, 0x009EF, WBP::Numeric},
    {0x009F0, 0x009F1, WBP::ALetter},
    {0x009FC, 0x009FC, WBP::ALetter},
    {0x009FE, 0x009FE, WBP::Extend},
    {0x00A01, 0x00A03, WBP::Extend},
    {0x00A05, 0x00A0A, WBP::ALetter},
    {0x00A0F, 0x00A10, WBP::ALetter},
    {0x00A13, 0x00A28, WBP::ALetter},
    {0x00A2A, 0x00A30, WBP::ALetter},
    {0x00A32, 0x00A33, WBP::ALetter},
    {0x00A35, 0x00A36, WBP::ALetter},
    {0x00A38, 0x00A39, WBP::ALetter},
    {0x00A3C, 0x00A3C, WBP::Extend},
    {0x00A3E, 0x00A42, WBP::Extend},
    {0x00A47, 0x00A48, WBP::Extend},
    {0x00A4B, 0x00A4D, WBP::Extend},
    {0x00A51, 0x00A51, WBP::Extend},
    {0x00A59, 0x00A5C, WBP::ALetter},
    {0x00A5E, 0x00A5E, WBP::ALetter},
    {0x00A66, 0x00A6F, WBP::Numeric},
    {0x00A70, 0x00A71, WBP::Extend},
    {0x00A72, 0x00A74, WBP::ALetter},
    {0x00A75, 0x00A75, WBP::Extend},
    {0x00A81, 0x00A83, WBP::Extend},
    {0x00A85, 0x00A8D, WBP::ALetter},
    {0x00A8F, 0x00A91, WBP::ALetter},
    {0x00A93, 0x00AA8, WBP::ALetter},
    {0x00AAA, 0x00AB0, WBP::ALetter},
    {0x00AB2, 0x00AB3, WBP::ALetter},
    {0x00AB5, 0x00AB9, WBP::ALetter},
    {0x00ABC, 0x00ABC, WBP::Extend},
    {0x00ABD, 0x00ABD, WBP::ALetter},
    {0x00ABE, 0x00AC5, WBP::Extend},
    {0x00AC7, 0x00AC9, WBP::Extend},
    {0x00ACB, 0x00ACD, WBP::Extend},
    {0x00AD0, 0x00AD0, WBP::ALetter},
    {0x00AE0, 0x00AE1, WBP::ALetter},
    {0x00AE2, 0x00AE3, WBP::Extend},
    {0x00AE6, 0x00AEF, WBP::Numeric},
    {0x00AF9, 0x00AF9, WBP::ALetter},
    {0x00AFA, 0x00AFF, WBP::Extend},
    {0x00B01, 0x00B03, WBP::Extend},
    {0x00B05, 0x00B0C, WBP::ALetter},
    {0x00B0F, 0x00B10, WBP::ALetter},
    {0x00B13, 0x00B28, WBP::ALetter},
    {0x00B2A, 0x00B30, WBP::ALetter},
    {0x00B32, 0x00B33, WBP::ALetter},
    {0x00B35, 0x00B39, WBP::ALetter},
    {0x00B3C, 0x00B3C, WBP::Extend},
    {0x00B3D, 0x00B3D, WBP::ALetter},
    {0x00B3E, 0x00B44, WBP::Extend},
    {0x00B47, 0x00B48, WBP::Extend},
    {0x00B4B, 0x00B4D, WBP::Extend},
    {0x00B55, 0x00B57, WBP::Extend},
    {0x00B5C, 0x00B5D, WBP::ALetter},
    {0x00B5F, 0x00B61, WBP::ALetter},
    {0x00B62, 0x00B63, WBP::Extend},
    {0x00B66, 0x00B6F, WBP::Numeric},
    {0x00B71, 0x00B71, WBP::ALetter},
    {0x00B82, 0x00B82, WBP::Extend},
    {0x00B83, 0x00B83, WBP::ALetter},
    {0x00B85, 0x00B8A, WBP::ALetter},
    {0x00B8E, 0x00B90, WBP::ALetter},
    {0x00B92, 0x00B95, WBP::ALetter},
    {0x00B99, 0x00B9A, WBP::ALetter},
    {0x00B9C, 0x00B9C, WBP::ALetter},
    {0x00B9E, 0x00B9F, WBP::ALetter},
    {0x00BA3, 0x00BA4, WBP::ALetter},
    {0x00BA8, 0x00BAA, WBP::ALetter},
    {0x00BAE, 0x00BB9, WBP::ALetter},
    {0x00BBE, 0x00BC2, WBP::Extend},
    {0x00BC6, 0x00BC8, WBP::Extend},
    {0x00BCA, 0x00BCD, WBP::Extend},
    {0x00BD0, 0x00BD0, WBP::ALetter},
    {0x00BD7, 0x00BD7, WBP::Extend},
    {0x00BE6, 0x00BEF, WBP::Numeric},
    {0x00C00, 0x00C04, WBP::Extend},
    {0x00C05, 0x00C0C, WBP::ALetter},
    {0x00C0E, 0x00C10, WBP::ALetter},
    {0x00C12, 0x00C28, WBP::ALetter},
    {0x00C2A, 0x00C39, WBP::ALetter},
    {0x00C3D, 0x00C3D, WBP::ALetter},
    {0x00C3E, 0x00C44, WBP::Extend},
    {0x00C46, 0x00C48, WBP::Extend},
    {0x00C4A, 0x00C4D, WBP::Extend},
    {0x00C55, 0x00C56, WBP::Extend},
    {0x00C58, 0x00C5A, WBP::ALetter},
    {0x00C60, 0x00C61, WBP::ALetter},
    {0x00C62, 0x00C63, WBP::Extend},
    {0x00C66, 0x00C6F, WBP::Numeric},
    {0x00C80, 0x00C80, WBP::ALetter},
    {0x00C81, 0x00C83, WBP::Extend},
    {0x00C85, 0x00C8C, WBP::ALetter},
    {0x00C8E, 0x00C90, WBP::ALetter},
    {0x00C92, 0x00CA8, WBP::ALetter},
    {0x00CAA, 0x00CB3, WBP::ALetter},
    {0x00CB5, 0x00CB9, WBP::ALetter},
    {0x00CBC, 0x00CBC, WBP::Extend},
    {0x00CBD, 0x00CBD, WBP::ALetter},
    {0x00CBE, 0x00CC4, WBP::Extend},
    {0x00CC6, 0x00CC8, WBP::Extend},
    {0x00CCA, 0x00CCD, WBP::Extend},
    {0x00CD5, 0x00CD6, WBP::Extend},
    {0x00CDE, 0x00CDE, WBP::ALetter},
    {0x00CE0, 0x00CE1, WBP::ALetter},
    {0x00CE2, 0x00CE3, WBP::Extend},
    {0x00CE6, 0x00CEF, WBP::Numeric},
    {0x00CF1, 0x00CF2, WBP::ALetter},
    {0x00D00, 0x00D03, WBP::Extend},
    {0x00D04, 0x00D0C, WBP::ALetter},
    {0x00D0E, 0x00D10, WBP::ALetter},
    {0x00D12, 0x00D3A, WBP::ALetter},
    {0x00D3B, 0x00D3C, WBP::Extend},
    {0x00D3D, 0x00D3D, WBP::ALetter},
    {0x00D3E, 0x00D44, WBP::Extend},
    {0x00D46, 0x00D48, WBP::Extend},
    {0x00D4A, 0x00D4D, WBP::Extend},
    {0x00D4E, 0x00D4E, WBP::ALetter},
    {0x00D54, 0x00D56, WBP::ALetter},
    {0x00D57, 0x00D57, WBP::Extend},
    {0x00D5F, 0x00D61, WBP::ALetter},
    {0x00D62, 0x00D63, WBP::Extend},
    {0x00D66, 0x00D6F, WBP::Numeric},
    {0x00D7A, 0x00D7F, WBP::ALetter},
    {0x00D81, 0x00D83, WBP::Extend},
    {0x00D85, 0x00D96, WBP::ALetter},
    {0x00D9A, 0x00DB1, WBP::ALetter},
    {0x00DB3, 0x00DBB, WBP::ALetter},
    {0x00DBD, 0x00DBD, WBP::ALetter},
    {0x00DC0, 0x00DC6, WBP::ALetter},
    {0x00DCA, 0x00DCA, WBP::Extend},
    {0x00DCF, 0x00DD4, WBP::Extend},
    {0x00DD6, 0x00DD6, WBP::Extend},
    {0x00DD8, 0x00DDF, WBP::Extend},
    {0x00DE6, 0x00DEF, WBP::Numeric},
    {0x00DF2, 0x00DF3, WBP::Extend},
    {0x00E31, 0x00E31, WBP::Extend},
    {0x00E34, 0x00E3A, WBP::Extend},
    {0x00E47, 0x00E4E, WBP::Extend},
    {0x00E50, 0x00E59, WBP::Numeric},
    {0x00EB1, 0x00EB1, WBP::Extend},
    {0x00EB4, 0x00EBC, WBP::Extend},
    {0x00EC8, 0x00ECD, WBP::Extend},
    {0x00ED0, 0x00ED9, WBP::Numeric},
    {0x00F00, 0x00F00, WBP::ALetter},
    {0x00F18, 0x00F19, WBP::Extend},
    {0x00F20, 0x00F29, WBP::Numeric},
    {0x00F35, 0x00F35, WBP::Extend},
    {0x00F37, 0x00F37, WBP::Extend},
    {0x00F39, 0x00F39, WBP::Extend},
    {0x00F3E, 0x00F3F, WBP::Extend},
    {0x00F40, 0x00F47, WBP::ALetter},
    {0x00F49, 0x00F6C, WBP::ALetter},
    {0x00F71, 0x00F84, WBP::Extend},
    {0x00F86, 0x00F87, WBP::Extend},
    {0x00F88, 0x00F8C, WBP::ALetter},
    {0x00F8D, 0x00F97, WBP::Extend},
    {0x00F99, 0x00FBC, WBP::Extend},
    {0x00FC6, 0x00FC6, WBP::Extend},
    {0x0102B, 0x0103E, WBP::Extend},
    {0x01040, 0x01049, WBP::Numeric},
    {0x01056, 0x01059, WBP::Extend},
    {0x0105E, 0x01060, WBP::Extend},
    {0x01062, 0x01064, WBP::Extend},
    {0x01067, 0x0106D, WBP::Extend},
    {0x01071, 0x01074, WBP::Extend},
    {0x01082, 0x0108D, WBP::Extend},
    {0x0108F, 0x0108F, WBP::Extend},
    {0x01090, 0x01099, WBP::Numeric},
    {0x0109A, 0x0109D, WBP::Extend},
    {0x010A0, 0x010C5, WBP::ALetter},
    {0x010C7, 0x010C7, WBP::ALetter},
    {0x010CD, 0x010CD, WBP::ALetter},
    {0x010D0, 0x010FA, WBP::ALetter},
    {0x010FC, 0x01248, WBP::ALetter},
    {0x0124A, 0x0124D, WBP::ALetter},
    {0x01250, 0x01256, WBP::ALetter},
    {0x01258, 0x01258, WBP::ALetter},
    {0x0125A, 0x0125D, WBP::ALetter},
    {0x01260, 0x01288, WBP::ALetter},
    {0x0128A, 0x0128D, WBP::ALetter},
    {0x01290, 0x012B0, WBP::ALetter},
    {0x012B2, 0x012B5, WBP::ALetter},
    {0x012B8, 0x012BE, WBP::ALetter},
    {0x012C0, 0x012C0, WBP::ALetter},
    {0x012C2, 0x012C5, WBP::ALetter},
    {0x012C8, 0x012D6, WBP::ALetter},
    {0x012D8, 0x01310, WBP::ALetter},
    {0x01312, 0x01315, WBP::ALetter},
    {0x01318, 0x0135A, WBP::ALetter},
    {0x0135D, 0x0135F, WBP::Extend},
    {0x01380, 0x0138F, WBP::ALetter},
    {0x013A0, 0x013F5, WBP::ALetter},
    {0x013F8, 0x013FD, WBP::ALetter},
    {0x01401, 0x0166C, WBP::ALetter},
    {0x0166F, 0x0167F, WBP::ALetter},
    {0x01680, 0x01680, WBP::WSegSpace},
    {0x01681, 0x0169A, WBP::ALetter},
    {0x016A0, 0x016EA, WBP::ALetter},
    {0x016EE, 0x016F8, WBP::ALetter},
    {0x01700, 0x0170C, WBP::ALetter},
    {0x0170E, 0x01711, WBP::ALetter},
    {0x01712, 0x01714, WBP::Extend},
    {0x01720, 0x01731, WBP::ALetter},
    {0x01732, 0x01734, WBP::Extend},
    {0x01740, 0x01751, WBP::ALetter},
    {0x01752, 0x01753, WBP::Extend},
    {0x01760, 0x0176C, WBP::ALetter},
    {0x0176E, 0x01770, WBP::ALetter},
    {0x01772, 0x01773, WBP::Extend},
    {0x017B4, 0x017D3, WBP::Extend},
    {0x017DD, 0x017DD, WBP::Extend},
    {0x017E0, 0x017E9, WBP::Numeric},
    {0x0180B, 0x0180D, WBP::Extend},
    {0x0180E, 0x0180E, WBP::Format},
    {0x01810, 0x01819, WBP::Numeric},
    {0x01820, 0x01878, WBP::ALetter},
    {0x01880, 0x01884, WBP::ALetter},
    {0x01885, 0x01886, WBP::Extend},
    {0x01887, 0x018A8, WBP::ALetter},
    {0x018A9, 0x018A9, WBP::Extend},
    {0x018AA, 0x018AA, WBP::ALetter},
    {0x018B0, 0x018F5, WBP::ALetter},
    {0x01900, 0x0191E, WBP::ALetter},
    {0x01920, 0x0192B, WBP::Extend},
    {0x01930, 0x0193B, WBP::Extend},
    {0x01946, 0x0194F, WBP::Numeric},
    {0x019D0, 0x019D9, WBP::Numeric},
    {0x01A00, 0x01A16, WBP::ALetter},
    {0x01A17, 0x01A1B, WBP::Extend},
    {0x01A55, 0x01A5E, WBP::Extend},
    {0x01A60, 0x01A7C, WBP::Extend},
    {0x01A7F, 0x01A7F, WBP::Extend},
    {0x01A80, 0x01A89, WBP::Numeric},
    {0x01A90, 0x01A99, WBP::Numeric},
    {0x01AB0, 0x01AC0, WBP::Extend},
    {0x01B00, 0x01B04, WBP::Extend},
    {0x01B05, 0x01B33, WBP::ALetter},
    {0x01B34, 0x01B44, WBP::Extend},
    {0x01B45, 0x01B4B, WBP::ALetter},
    {0x01B50, 0x01B59, WBP::Numeric},
    {0x01B6B, 0x01B73, WBP::Extend},
    {0x01B80, 0x01B82, WBP::Extend},
    {0x01B83, 0x01BA0, WBP::ALetter},
    {0x01BA1, 0x01BAD, WBP::Extend},
    {0x01BAE, 0x01BAF, WBP::ALetter},
    {0x01BB0, 0x01BB9, WBP::Numeric},
    {0x01BBA, 0x01BE5, WBP::ALetter},
    {0x01BE6, 0x01BF3, WBP::Extend},
    {0x01C00, 0x01C23, WBP::ALetter},
    {0x01C24, 0x01C37, WBP::Extend},
    {0x01C40, 0x01C49, WBP::Numeric},
    {0x01C4D, 0x01C4F, WBP::ALetter},
    {0x01C50, 0x01C59, WBP::Numeric},
    {0x01C5A, 0x01C7D, WBP::ALetter},
    {0x01C80, 0x01C88, WBP::ALetter},
    {0x01C90, 0x01CBA, WBP::ALetter},
    {0x01CBD, 0x01CBF, WBP::ALetter},
    {0x01CD0, 0x01CD2, WBP::Extend},
    {0x01CD4, 0x01CE8, WBP::Extend},
    {0x01CE9, 0x01CEC, WBP::ALetter},
    {0x01CED, 0x01CED, WBP::Extend},
    {0x01CEE, 0x01CF3, WBP::ALetter},
    {0x01CF4, 0x01CF4, WBP::Extend},
    {0x01CF5, 0x01CF6, WBP::ALetter},
    {0x01CF7, 0x01CF9, WBP::Extend},
    {0x01CFA, 0x01CFA, WBP::ALetter},
    {0x01D00, 0x01DBF, WBP::ALetter},
    {0x01DC0, 0x01DF9, WBP::Extend},
    {0x01DFB, 0x01DFF, WBP::Extend},
    {0x01E00, 0x01F15, WBP::ALetter},
    {0x01F18, 0x01F1D, WBP::ALetter},
    {0x01F20, 0x01F45, WBP::ALetter},
    {0x01F48, 0x01F4D, WBP::ALetter},
    {0x01F50, 0x01F57, WBP::ALetter},
    {0x01F59, 0x01F59, WBP::ALetter},
    {0x01F5B, 0x01F5B, WBP::ALetter},
    {0x01F5D, 0x01F5D, WBP::ALetter},
    {0x01F5F, 0x01F7D, WBP::ALetter},
    {0x01F80, 0x01FB4, WBP::ALetter},
    {0x01FB6, 0x01FBC, WBP::ALetter},
    {0x01FBE, 0x01FBE, WBP::ALetter},
    {0x01FC2, 0x01FC4, WBP::ALetter},
    {0x01FC6, 0x01FCC, WBP::ALetter},
    {0x01FD0, 0x01FD3, WBP::ALetter},
    {0x01FD6, 0x01FDB, WBP::ALetter},
    {0x01FE0, 0x01FEC, WBP::ALetter},
    {0x01FF2, 0x01FF4, WBP::ALetter},
    {0x01FF6, 0x01FFC, WBP::ALetter},
    {0x02000, 0x02006, WBP::WSegSpace},
    {0x02008, 0x0200A, WBP::WSegSpace},
    {0x0200C, 0x0200C, WBP::Extend},
    {0x0200D, 0x0200D, WBP::ZWJ},
    {0x0200E, 0x0200F, WBP::Format},
    {0x02018, 0x02019, WBP::MidNumLet},
    {0x02024, 0x02024, WBP::MidNumLet},
    {0x02027, 0x02027, WBP::MidLetter},
    {0x02028, 0x02029, WBP::Newline},
    {0x0202A, 0x0202E, WBP::Format},
    {0x0202F, 0x0202F, WBP::ExtendNumLet},
    {0x0203F, 0x02040, WBP::ExtendNumLet},
    {0x02044, 0x02044, WBP::MidNum},
    {0x02054, 0x02054, WBP::ExtendNumLet},
    {0x0205F, 0x0205F, WBP::WSegSpace},
    {0x02060, 0x02064, WBP::Format},
    {0x02066, 0x0206F, WBP::Format},
    {0x02071, 0x02071, WBP::ALetter},
    {0x0207F, 0x0207F, WBP::ALetter},
    {0x02090, 0x0209C, WBP::ALetter},
    {0x020D0, 0x020F0, WBP::Extend},
    {0x02102, 0x02102, WBP::ALetter},
    {0x02107, 0x02107, WBP::ALetter},
    {0x0210A, 0x02113, WBP::ALetter},
    {0x02115, 0x02115, WBP::ALetter},
    {0x02119, 0x0211D, WBP::ALetter},
    {0x02124, 0x02124, WBP::ALetter},
    {0x02126, 0x02126, WBP::ALetter},
    {0x02128, 0x02128, WBP::ALetter},
    {0x0212A, 0x0212D, WBP::ALetter},
    {0x0212F, 0x02139, WBP::ALetter},
    {0x0213C, 0x0213F, WBP::ALetter},
    {0x02145, 0x02149, WBP::ALetter},
    {0x0214E, 0x0214E, WBP::ALetter},
    {0x02160, 0x02188, WBP::ALetter},
    {0x024B6, 0x024E9, WBP::ALetter},
    {0x02C00, 0x02C2E, WBP::ALetter},
    {0x02C30, 0x02C5E, WBP::ALetter},
    {0x02C60, 0x02CE4, WBP::ALetter},
    {0x02CEB, 0x02CEE, WBP::ALetter},
    {0x02CEF, 0x02CF1, WBP::Extend},
    {0x02CF2, 0x02CF3, WBP::ALetter},
    {0x02D00, 0x02D25, WBP::ALetter},
    {0x02D27, 0x02D27, WBP::ALetter},
    {0x02D2D, 0x02D2D, WBP::ALetter},
    {0x02D30, 0x02D67, WBP::ALetter},
    {0x02D6F, 0x02D6F, WBP::ALetter},
    {0x02D7F, 0x02D7F, WBP::Extend},
    {0x02D80, 0x02D96, WBP::ALetter},
    {0x02DA0, 0x02DA6, WBP::ALetter},
    {0x02DA8, 0x02DAE, WBP::ALetter},
    {0x02DB0, 0x02DB6, WBP::ALetter},
    {0x02DB8, 0x02DBE, WBP::ALetter},
    {0x02DC0, 0x02DC6, WBP::ALetter},
    {0x02DC8, 0x02DCE, WBP::ALetter},
    {0x02DD0, 0x02DD6, WBP::ALetter},
    {0x02DD8, 0x02DDE, WBP::ALetter},
    {0x02DE0, 0x02DFF, WBP::Extend},
    {0x02E2F, 0x02E2F, WBP::ALetter},
    {0x03000, 0x03000, WBP::WSegSpace},
    {0x03005, 0x03005, WBP::ALetter},
    {0x0302A, 0x0302F, WBP::Extend},
    {0x03031, 0x03035, WBP::Katakana},
    {0x0303B, 0x0303C, WBP::ALetter},
    {0x03099, 0x0309A, WBP::Extend},
    {0x0309B, 0x0309C, WBP::Katakana},
    {0x030A0, 0x030FA, WBP::Katakana},
    {0x030FC, 0x030FF, WBP::Katakana},
    {0x03105, 0x0312F, WBP::ALetter},
    {0x03131, 0x0318E, WBP::ALetter},
    {0x031A0, 0x031BF, WBP::ALetter},
    {0x031F0, 0x031FF, WBP::Katakana},
    {0x032D0, 0x032FE, WBP::Katakana},
    {0x03300, 0x03357, WBP::Katakana},
    {0x0A000, 0x0A48C, WBP::ALetter},
    {0x0A4D0, 0x0A4FD, WBP::ALetter},
    {0x0A500, 0x0A60C, WBP::ALetter},
    {0x0A610, 0x0A61F, WBP::ALetter},
    {0x0A620, 0x0A629, WBP::Numeric},
    {0x0A62A, 0x0A62B, WBP::ALetter},
    {0x0A640, 0x0A66E, WBP::ALetter},
    {0x0A66F, 0x0A672, WBP::Extend},
    {0x0A674, 0x0A67D, WBP::Extend},
    {0x0A67F, 0x0A69D, WBP::ALetter},
    {0x0A69E, 0x0A69F, WBP::Extend},
    {0x0A6A0, 0x0A6EF, WBP::ALetter},
    {0x0A6F0, 0x0A6F1, WBP::Extend},
    {0x0A708, 0x0A7BF, WBP::ALetter},
    {0x0A7C2, 0x0A7CA, WBP::ALetter},
    {0x0A7F5, 0x0A801, WBP::ALetter},
    {0x0A802, 0x0A802, WBP::Extend},
    {0x0A803, 0x0A805, WBP::ALetter},
    {0x0A806, 0x0A806, WBP::Extend},
    {0x0A807, 0x0A80A, WBP::ALetter},
    {0x0A80B, 0x0A80B, WBP::Extend},
    {0x0A80C, 0x0A822, WBP::ALetter},
    {0x0A823, 0x0A827, WBP::Extend},
    {0x0A82C, 0x0A82C, WBP::Extend},
    {0x0A840, 0x0A873, WBP::ALetter},
    {0x0A880, 0x0A881, WBP::Extend},
    {0x0A882, 0x0A8B3, WBP::ALetter},
    {0x0A8B4, 0x0A8C5, WBP::Extend},
    {0x0A8D0, 0x0A8D9, WBP::Numeric},
    {0x0A8E0, 0x0A8F1, WBP::Extend},
    {0x0A8F2, 0x0A8F7, WBP::ALetter},
    {0x0A8FB, 0x0A8FB, WBP::ALetter},
    {0x0A8FD, 0x0A8FE, WBP::ALetter},
    {0x0A8FF, 0x0A8FF, WBP::Extend},
    {0x0A900, 0x0A909, WBP::Numeric},
    {0x0A90A, 0x0A925, WBP::ALetter},
    {0x0A926, 0x0A92D, WBP::Extend},
    {0x0A930, 0x0A946, WBP::ALetter},
    {0x0A947, 0x0A953, WBP::Extend},
    {0x0A960, 0x0A97C, WBP::ALetter},
    {0x0A980, 0x0A983, WBP::Extend},
    {0x0A984, 0x0A9B2, WBP::ALetter},
    {0x0A9B3, 0x0A9C0, WBP::Extend},
    {0x0A9CF, 0x0A9CF, WBP::ALetter},
    {0x0A9D0, 0x0A9D9, WBP::Numeric},
    {0x0A9E5, 0x0A9E5, WBP::Extend},
    {0x0A9F0, 0x0A9F9, WBP::Numeric},
    {0x0AA00, 0x0AA28, WBP::ALetter},
    {0x0AA29, 0x0AA36, WBP::Extend},
    {0x0AA40, 0x0AA42, WBP::ALetter},
    {0x0AA43, 0x0AA43, WBP::Extend},
    {0x0AA44, 0x0AA4B, WBP::ALetter},
    {0x0AA4C, 0x0AA4D, WBP::Extend},
    {0x0AA50, 0x0AA59, WBP::Numeric},
    {0x0AA7B, 0x0AA7D, WBP::Extend},
    {0x0AAB0, 0x0AAB0, WBP::Extend},
    {0x0AAB2, 0x0AAB4, WBP::Extend},
    {0x0AAB7, 0x0AAB8, WBP::Extend},
    {0x0AABE, 0x0AABF, WBP::Extend},
    {0x0AAC1, 0x0AAC1, WBP::Extend},
    {0x0AAE0, 0x0AAEA, WBP::ALetter},
    {0x0AAEB, 0x0AAEF, WBP::Extend},
    {0x0AAF2, 0x0AAF4, WBP::ALetter},
    {0x0AAF5, 0x0AAF6, WBP::Extend},
    {0x0AB01, 0x0AB06, WBP::ALetter},
    {0x0AB09, 0x0AB0E, WBP::ALetter},
    {0x0AB11, 0x0AB16, WBP::ALetter},
    {0x0AB20, 0x0AB26, WBP::ALetter},
    {0x0AB28, 0x0AB2E, WBP::ALetter},
    {0x0AB30, 0x0AB69, WBP::ALetter},
    {0x0AB70, 0x0ABE2, WBP::ALetter},
    {0x0ABE3, 0x0ABEA, WBP::Extend},
    {0x0ABEC, 0x0ABED, WBP::Extend},
    {0x0ABF0, 0x0ABF9, WBP::Numeric},
    {0x0AC00, 0x0D7A3, WBP::ALetter},
    {0x0D7B0, 0x0D7C6, WBP::ALetter},
    {0x0D7CB, 0x0D7FB, WBP::ALetter},
    {0x0FB00, 0x0FB06, WBP::ALetter},
    {0x0FB13, 0x0FB17, WBP::ALetter},
    {0x0FB1D, 0x0FB1D, WBP::Hebrew_Letter},
    {0x0FB1E, 0x0FB1E, WBP::Extend},
    {0x0FB1F, 0x0FB28, WBP::Hebrew_Letter},
    {0x0FB2A, 0x0FB36, WBP::Hebrew_Letter},
    {0x0FB38, 0x0FB3C, WBP::Hebrew_Letter},
    {0x0FB3E, 0x0FB3E, WBP::Hebrew_Letter},
    {0x0FB40, 0x0FB41, WBP::Hebrew_Letter},
    {0x0FB43, 0x0FB44, WBP::Hebrew_Letter},
    {0x0FB46, 0x0FB4F, WBP::Hebrew_Letter},
    {0x0FB50, 0x0FBB1, WBP::ALetter},
    {0x0FBD3, 0x0FD3D, WBP::ALetter},
    {0x0FD50, 0x0FD8F, WBP::ALetter},
    {0x0FD92, 0x0FDC7, WBP::ALetter},
    {0x0FDF0, 0x0FDFB, WBP::ALetter},
    {0x0FE00, 0x0FE0F, WBP::Extend},
    {0x0FE10, 0x0FE10, WBP::MidNum},
    {0x0FE13, 0x0FE13, WBP::MidLetter},
    {0x0FE14, 0x0FE14, WBP::MidNum},
    {0x0FE20, 0x0FE2F, WBP::Extend},
    {0x0FE33, 0x0FE34, WBP::ExtendNumLet},
    {0x0FE4D, 0x0FE4F, WBP::ExtendNumLet},
    {0x0FE50, 0x0FE50, WBP::MidNum},
    {0x0FE52, 0x0FE52, WBP::MidNumLet},
    {0x0FE54, 0x0FE54, WBP::MidNum},
    {0x0FE55, 0x0FE55, WBP::MidLetter},
    {0x0FE70, 0x0FE74, WBP::ALetter},
    {0x0FE76, 0x0FEFC, WBP::ALetter},
    {0x0FEFF, 0x0FEFF, WBP::Format},
    {0x0FF07, 0x0FF07, WBP::MidNumLet},
    {0x0FF0C, 0x0FF0C, WBP::MidNum},
    {0x0FF0E, 0x0FF0E, WBP::MidNumLet},
    {0x0FF10, 0x0FF19, WBP::Numeric},
    {0x0FF1A, 0x0FF1A, WBP::MidLetter},
    {0x0FF1B, 0x0FF1B, WBP::MidNum},
    {0x0FF21, 0x0FF3A, WBP::ALetter},
    {0x0FF3F, 0x0FF3F, WBP::ExtendNumLet},
    {0x0FF41, 0x0FF5A, WBP::ALetter},
    {0x0FF66, 0x0FF9D, WBP::Katakana},
    {0x0FF9E, 0x0FF9F, WBP::Extend},
    {0x0FFA0, 0x0FFBE, WBP::ALetter},
    {0x0FFC2, 0x0FFC7, WBP::ALetter},
    {0x0FFCA, 0x0FFCF, WBP::ALetter},
    {0x0FFD2, 0x0FFD7, WBP::ALetter},
    {0x0FFDA, 0x0FFDC, WBP::ALetter},
    {0x0FFF9, 0x0FFFB, WBP::Format},
    {0x10000, 0x1000B, WBP::ALetter},
    {0x1000D, 0x10026, WBP::ALetter},
    {0x10028, 0x1003A, WBP::ALetter},
    {0x1003C, 0x1003D, WBP::ALetter},
    {0x1003F, 0x1004D, WBP::ALetter},
    {0x10050, 0x1005D, WBP::ALetter},
    {0x10080, 0x100FA, WBP::ALetter},
    {0x10140, 0x10174, WBP::ALetter},
    {0x101FD, 0x101FD, WBP::Extend},
    {0x10280, 0x1029C, WBP::ALetter},
    {0x102A0, 0x102D0, WBP::ALetter},
    {0x102E0, 0x102E0, WBP::Extend},
    {0x10300, 0x1031F, WBP::ALetter},
    {0x1032D, 0x1034A, WBP::ALetter},
    {0x10350, 0x10375, WBP::ALetter},
    {0x10376, 0x1037A, WBP::Extend},
    {0x10380, 0x1039D, WBP::ALetter},
    {0x103A0, 0x103C3, WBP::ALetter},
    {0x103C8, 0x103CF, WBP::ALetter},
    {0x103D1, 0x103D5, WBP::ALetter},
    {0x10400, 0x1049D, WBP::ALetter},
    {0x104A0, 0x104A9, WBP::Numeric},
    {0x104B0, 0x104D3, WBP::ALetter},
    {0x104D8, 0x104FB, WBP::ALetter},
    {0x10500, 0x10527, WBP::ALetter},
    {0x10530, 0x10563, WBP::ALetter},
    {0x10600, 0x10736, WBP::ALetter},
    {0x10740, 0x10755, WBP::ALetter},
    {0x10760, 0x10767, WBP::ALetter},
    {0x10800, 0x10805, WBP::ALetter},
    {0x10808, 0x10808, WBP::ALetter},
    {0x1080A, 0x10835, WBP::ALetter},
    {0x10837, 0x10838, WBP::ALetter},
    {0x1083C, 0x1083C, WBP::ALetter},
    {0x1083F, 0x10855, WBP::ALetter},
    {0x10860, 0x10876, WBP::ALetter},
    {0x10880, 0x1089E, WBP::ALetter},
    {0x108E0, 0x108F2, WBP::ALetter},
    {0x108F4, 0x108F5, WBP::ALetter},
    {0x10900, 0x10915, WBP::ALetter},
    {0x10920, 0x10939, WBP::ALetter},
    {0x10980, 0x109B7, WBP::ALetter},
    {0x109BE, 0x109BF, WBP::ALetter},
    {0x10A00, 0x10A00, WBP::ALetter},
    {0x10A01, 0x10A03, WBP::Extend},
    {0x10A05, 0x10A06, WBP::Extend},
    {0x10A0C, 0x10A0F, WBP::Extend},
    {0x10A10, 0x10A13, WBP::ALetter},
    {0x10A15, 0x10A17, WBP::ALetter},
    {0x10A19, 0x10A35, WBP::ALetter},
    {0x10A38, 0x10A3A, WBP::Extend},
    {0x10A3F, 0x10A3F, WBP::Extend},
    {0x10A60, 0x10A7C, WBP::ALetter},
    {0x10A80, 0x10A9C, WBP::ALetter},
    {0x10AC0, 0x10AC7, WBP::ALetter},
    {0x10AC9, 0x10AE4, WBP::ALetter},
    {0x10AE5, 0x10AE6, WBP::Extend},
    {0x10B00, 0x10B35, WBP::ALetter},
    {0x10B40, 0x10B55, WBP::ALetter},
    {0x10B60, 0x10B72, WBP::ALetter},
    {0x10B80, 0x10B91, WBP::ALetter},
    {0x10C00, 0x10C48, WBP::ALetter},
    {0x10C80, 0x10CB2, WBP::ALetter},
    {0x10CC0, 0x10CF2, WBP::ALetter},
    {0x10D00, 0x10D23, WBP::ALetter},
    {0x10D24, 0x10D27, WBP::Extend},
    {0x10D30, 0x10D39, WBP::Numeric},
    {0x10E80, 0x10EA9, WBP::ALetter},
    {0x10EAB, 0x10EAC, WBP::Extend},
    {0x10EB0, 0x10EB1, WBP::ALetter},
    {0x10F00, 0x10F1C, WBP::ALetter},
    {0x10F27, 0x10F27, WBP::ALetter},
    {0x10F30, 0x10F45, WBP::ALetter},
    {0x10F46, 0x10F50, WBP::Extend},
    {0x10FB0, 0x10FC4, WBP::ALetter},
    {0x10FE0, 0x10FF6, WBP::ALetter},
    {0x11000, 0x11002, WBP::Extend},
    {0x11003, 0x11037, WBP::ALetter},
    {0x11038, 0x11046, WBP::Extend},
    {0x11066, 0x1106F, WBP::Numeric},
    {0x1107F, 0x11082, WBP::Extend},
    {0x11083, 0x110AF, WBP::ALetter},
    {0x110B0, 0x110BA, WBP::Extend},
    {0x110BD, 0x110BD, WBP::Format},
    {0x110CD, 0x110CD, WBP::Format},
    {0x110D0, 0x110E8, WBP::ALetter},
    {0x110F0, 0x110F9, WBP::Numeric},
    {0x11100, 0x11102, WBP::Extend},
    {0x11103, 0x11126, WBP::ALetter},
    {0x11127, 0x11134, WBP::Extend},
    {0x11136, 0x1113F, WBP::Numeric},
    {0x11144, 0x11144, WBP::ALetter},
    {0x11145, 0x11146, WBP::Extend},
    {0x11147, 0x11147, WBP::ALetter},
    {0x11150, 0x11172, WBP::ALetter},
    {0x11173, 0x11173, WBP::Extend},
    {0x11176, 0x11176, WBP::ALetter},
    {0x11180, 0x11182, WBP::Extend},
    {0x11183, 0x111B2, WBP::ALetter},
    {0x111B3, 0x111C0, WBP::Extend},
    {0x111C1, 0x111C4, WBP::ALetter},
    {0x111C9, 0x111CC, WBP::Extend},
    {0x111CE, 0x111CF, WBP::Extend},
    {0x111D0, 0x111D9, WBP::Numeric},
    {0x111DA, 0x111DA, WBP::ALetter},
    {0x111DC, 0x111DC, WBP::ALetter},
    {0x11200, 0x11211, WBP::ALetter},
    {0x11213, 0x1122B, WBP::ALetter},
    {0x1122C, 0x11237, WBP::Extend},
    {0x1123E, 0x1123E, WBP::Extend},
    {0x11280, 0x11286, WBP::ALetter},
    {0x11288, 0x11288, WBP::ALetter},
    {0x1128A, 0x1128D, WBP::ALetter},
    {0x1128F, 0x1129D, WBP::ALetter},
    {0x1129F, 0x112A8, WBP::ALetter},
    {0x112B0, 0x112DE, WBP::ALetter},
    {0x112DF, 0x112EA, WBP::Extend},
    {0x112F0, 0x112F9, WBP::Numeric},
    {0x11300, 0x11303, WBP::Extend},
    {0x11305, 0x1130C, WBP::ALetter},
    {0x1130F, 0x11310, WBP::ALetter},
    {0x11313, 0x11328, WBP::ALetter},
    {0x1132A, 0x11330, WBP::ALetter},
    {0x11332, 0x11333, WBP::ALetter},
    {0x11335, 0x11339, WBP::ALetter},
    {0x1133B, 0x1133C, WBP::Extend},
    {0x1133D, 0x1133D, WBP::ALetter},
    {0x1133E, 0x11344, WBP::Extend},
    {0x11347, 0x11348, WBP::Extend},
    {0x1134B, 0x1134D, WBP::Extend},
    {0x11350, 0x11350, WBP::ALetter},
    {0x11357, 0x11357, WBP::Extend},
    {0x1135D, 0x11361, WBP::ALetter},
    {0x11362, 0x11363, WBP::Extend},
    {0x11366, 0x1136C, WBP::Extend},
    {0x11370, 0x11374, WBP::Extend},
    {0x11400, 0x11434, WBP::ALetter},
    {0x11435, 0x11446, WBP::Extend},
    {0x11447, 0x1144A, WBP::ALetter},
    {0x11450, 0x11459, WBP::Numeric},
    {0x1145E, 0x1145E, WBP::Extend},
    {0x1145F, 0x11461, WBP::ALetter},
    {0x11480, 0x114AF, WBP::ALetter},
    {0x114B0, 0x114C3, WBP::Extend},
    {0x114C4, 0x114C5, WBP::ALetter},
    {0x114C7, 0x114C7, WBP::ALetter},
    {0x114D0, 0x114D9, WBP::Numeric},
    {0x11580, 0x115AE, WBP::ALetter},
    {0x115AF, 0x115B5, WBP::Extend},
    {0x115B8, 0x115C0, WBP::Extend},
    {0x115D8, 0x115DB, WBP::ALetter},
    {0x115DC, 0x115DD, WBP::Extend},
    {0x11600, 0x1162F, WBP::ALetter},
    {0x11630, 0x11640, WBP::Extend},
    {0x11644, 0x11644, WBP::ALetter},
    {0x11650, 0x11659, WBP::Numeric},
    {0x11680, 0x116AA, WBP::ALetter},
    {0x116AB, 0x116B7, WBP::Extend},
    {0x116B8, 0x116B8, WBP::ALetter},
    {0x116C0, 0x116C9, WBP::Numeric},
    {0x1171D, 0x1172B, WBP::Extend},
    {0x11730, 0x11739, WBP::Numeric},
    {0x11800, 0x1182B, WBP::ALetter},
    {0x1182C, 0x1183A, WBP::Extend},
    {0x118A0, 0x118DF, WBP::ALetter},
    {0x118E0, 0x118E9, WBP::Numeric},
    {0x118FF, 0x11906, WBP::ALetter},
    {0x11909, 0x11909, WBP::ALetter},
    {0x1190C, 0x11913, WBP::ALetter},
    {0x11915, 0x11916, WBP::ALetter},
    {0x11918, 0x1192F, WBP::ALetter},
    {0x11930, 0x11935, WBP::Extend},
    {0x11937, 0x11938, WBP::Extend},
    {0x1193B, 0x1193E, WBP::Extend},
    {0x1193F, 0x1193F, WBP::ALetter},
    {0x11940, 0x11940, WBP::Extend},
    {0x11941, 0x11941, WBP::ALetter},
    {0x11942, 0x11943, WBP::Extend},
    {0x11950, 0x11959, WBP::Numeric},
    {0x119A0, 0x119A7, WBP::ALetter},
    {0x119AA, 0x119D0, WBP::ALetter},
    {0x119D1, 0x119D7, WBP::Extend},
    {0x119DA, 0x119E0, WBP::Extend},
    {0x119E1, 0x119E1, WBP::ALetter},
    {0x119E3, 0x119E3, WBP::ALetter},
    {0x119E4, 0x119E4, WBP::Extend},
    {0x11A00, 0x11A00, WBP::ALetter},
    {0x11A01, 0x11A0A, WBP::Extend},
    {0x11A0B, 0x11A32, WBP::ALetter},
    {0x11A33, 0x11A39, WBP::Extend},
    {0x11A3A, 0x11A3A, WBP::ALetter},
    {0x11A3B, 0x11A3E, WBP::Extend},
    {0x11A47, 0x11A47, WBP::Extend},
    {0x11A50, 0x11A50, WBP::ALetter},
    {0x11A51, 0x11A5B, WBP::Extend},
    {0x11A5C, 0x11A89, WBP::ALetter},
    {0x11A8A, 0x11A99, WBP::Extend},
    {0x11A9D, 0x11A9D, WBP::ALetter},
    {0x11AC0, 0x11AF8, WBP::ALetter},
    {0x11C00, 0x11C08, WBP::ALetter},
    {0x11C0A, 0x11C2E, WBP::ALetter},
    {0x11C2F, 0x11C36, WBP::Extend},
    {0x11C38, 0x11C3F, WBP::Extend},
    {0x11C40, 0x11C40, WBP::ALetter},
    {0x11C50, 0x11C59, WBP::Numeric},
    {0x11C72, 0x11C8F, WBP::ALetter},
    {0x11C92, 0x11CA7, WBP::Extend},
    {0x11CA9, 0x11CB6, WBP::Extend},
    {0x11D00, 0x11D06, WBP::ALetter},
    {0x11D08, 0x11D09, WBP::ALetter},
    {0x11D0B, 0x11D30, WBP::ALetter},
    {0x11D31, 0x11D36, WBP::Extend},
    {0x11D3A, 0x11D3A, WBP::Extend},
    {0x11D3C, 0x11D3D, WBP::Extend},
    {0x11D3F, 0x11D45, WBP::Extend},
    {0x11D46, 0x11D46, WBP::ALetter},
    {0x11D47, 0x11D47, WBP::Extend},
    {0x11D50, 0x11D59, WBP::Numeric},
    {0x11D60, 0x11D65, WBP::ALetter},
    {0x11D67, 0x11D68, WBP::ALetter},
    {0x11D6A, 0x11D89, WBP::ALetter},
    {0x11D8A, 0x11D8E, WBP::Extend},
    {0x11D90, 0x11D91, WBP::Extend},
    {0x11D93, 0x11D97, WBP::Extend},
    {0x11D98, 0x11D98, WBP::ALetter},
    {0x11DA0, 0x11DA9, WBP::Numeric},
    {0x11EE0, 0x11EF2, WBP::ALetter},
    {0x11EF3, 0x11EF6, WBP::Extend},
    {0x11FB0, 0x11FB0, WBP::ALetter},
    {0x12000, 0x12399, WBP::ALetter},
    {0x12400, 0x1246E, WBP::ALetter},
    {0x12480, 0x12543, WBP::ALetter},
    {0x13000, 0x1342E, WBP::ALetter},
    {0x13430, 0x13438, WBP::Format},
    {0x14400, 0x14646, WBP::ALetter},
    {0x16800, 0x16A38, WBP::ALetter},
    {0x16A40, 0x16A5E, WBP::ALetter},
    {0x16A60, 0x16A69, WBP::Numeric},
    {0x16AD0, 0x16AED, WBP::ALetter},
    {0x16AF0, 0x16AF4, WBP::Extend},
    {0x16B00, 0x16B2F, WBP::ALetter},
    {0x16B30, 0x16B36, WBP::Extend},
    {0x16B40, 0x16B43, WBP::ALetter},
    {0x16B50, 0x16B59, WBP::Numeric},
    {0x16B63, 0x16B77, WBP::ALetter},
    {0x16B7D, 0x16B8F, WBP::ALetter},
    {0x16E40, 0x16E7F, WBP::ALetter},
    {0x16F00, 0x16F4A, WBP::ALetter},
    {0x16F4F, 0x16F4F, WBP::Extend},
    {0x16F50, 0x16F50, WBP::ALetter},
    {0x16F51, 0x16F87, WBP::Extend},
    {0x16F8F, 0x16F92, WBP::Extend},
    {0x16F93, 0x16F9F, WBP::ALetter},
    {0x16FE0, 0x16FE1, WBP::ALetter},
    {0x16FE3, 0x16FE3, WBP::ALetter},
    {0x16FE4, 0x16FE4, WBP::Extend},
    {0x16FF0, 0x16FF1, WBP::Extend},
    {0x1B000, 0x1B000, WBP::Katakana},
    {0x1B164, 0x1B167, WBP::Katakana},
    {0x1BC00, 0x1BC6A, WBP::ALetter},
    {0x1BC70, 0x1BC7C, WBP::ALetter},
    {0x1BC80, 0x1BC88, WBP::ALetter},
    {0x1BC90, 0x1BC99, WBP::ALetter},
    {0x1BC9D, 0x1BC9E, WBP::Extend},
    {0x1BCA0, 0x1BCA3, WBP::Format},
    {0x1D165, 0x1D169, WBP::Extend},
    {0x1D16D, 0x1D172, WBP::Extend},
    {0x1D173, 0x1D17A, WBP::Format},
    {0x1D17B, 0x1D182, WBP::Extend},
    {0x1D185, 0x1D18B, WBP::Extend},
    {0x1D1AA, 0x1D1AD, WBP::Extend},
    {0x1D242, 0x1D244, WBP::Extend},
    {0x1D400, 0x1D454, WBP::ALetter},
    {0x1D456, 0x1D49C, WBP::ALetter},
    {0x1D49E, 0x1D49F, WBP::ALetter},
    {0x1D4A2, 0x1D4A2, WBP::ALetter},
    {0x1D4A5, 0x1D4A6, WBP::ALetter},
    {0x1D4A9, 0x1D4AC, WBP::ALetter},
    {0x1D4AE, 0x1D4B9, WBP::ALetter},
    {0x1D4BB, 0x1D4BB, WBP::ALetter},
    {0x1D4BD, 0x1D4C3, WBP::ALetter},
    {0x1D4C5, 0x1D505, WBP::ALetter},
    {0x1D507, 0x1D50A, WBP::ALetter},
    {0x1D50D, 0x1D514, WBP::ALetter},
    {0x1D516, 0x1D51C, WBP::ALetter},
    {0x1D51E, 0x1D539, WBP::ALetter},
    {0x1D53B, 0x1D53E, WBP::ALetter},
    {0x1D540, 0x1D544, WBP::ALetter},
    {0x1D546, 0x1D546, WBP::ALetter},
    {0x1D54A, 0x1D550, WBP::ALetter},
    {0x1D552, 0x1D6A5, WBP::ALetter},
    {0x1D6A8, 0x1D6C0, WBP::ALetter},
    {0x1D6C2, 0x1D6DA, WBP::ALetter},
    {0x1D6DC, 0x1D6FA, WBP::ALetter},
    {0x1D6FC, 0x1D714, WBP::ALetter},
    {0x1D716, 0x1D734, WBP::ALetter},
    {0x1D736, 0x1D74E, WBP::ALetter},
    {0x1D750, 0x1D76E, WBP::ALetter},
    {0x1D770, 0x1D788, WBP::ALetter},
    {0x1D78A, 0x1D7A8, WBP::ALetter},
    {0x1D7AA, 0x1D7C2, WBP::ALetter},
    {0x1D7C4, 0x1D7CB, WBP::ALetter},
    {0x1D7CE, 0x1D7FF, WBP::Numeric},
    {0x1DA00, 0x1DA36, WBP::Extend},
    {0x1DA3B, 0x1DA6C, WBP::Extend},
    {0x1DA75, 0x1DA75, WBP::Extend},
    {0x1DA84, 0x1DA84, WBP::Extend},
    {0x1DA9B, 0x1DA9F, WBP::Extend},
    {0x1DAA1, 0x1DAAF, WBP::Extend},
    {0x1E000, 0x1E006, WBP::Extend},
    {0x1E008, 0x1E018, WBP::Extend},
    {0x1E01B, 0x1E021, WBP::Extend},
    {0x1E023, 0x1E024, WBP::Extend},
    {0x1E026, 0x1E02A, WBP::Extend},
    {0x1E100, 0x1E12C, WBP::ALetter},
    {0x1E130, 0x1E136, WBP::Extend},
    {0x1E137, 0x1E13D, WBP::ALetter},
    {0x1E140, 0x1E149, WBP::Numeric},
    {0x1E14E, 0x1E14E, WBP::ALetter},
    {0x1E2C0, 0x1E2EB, WBP::ALetter},
    {0x1E2EC, 0x1E2EF, WBP::Extend},
    {0x1E2F0, 0x1E2F9, WBP::Numeric},
    {0x1E800, 0x1E8C4, WBP::ALetter},
    {0x1E8D0, 0x1E8D6, WBP::Extend},
    {0x1E900, 0x1E943, WBP::ALetter},
    {0x1E944, 0x1E94A, WBP::Extend},
    {0x1E94B, 0x1E94B, WBP::ALetter},
    {0x1E950, 0x1E959, WBP::Numeric},
    {0x1EE00, 0x1EE03, WBP::ALetter},
    {0x1EE05, 0x1EE1F, WBP::ALetter},
    {0x1EE21, 0x1EE22, WBP::ALetter},
    {0x1EE24, 0x1EE24, WBP::ALetter},
    {0x1EE27, 0x1EE27, WBP::ALetter},
    {0x1EE29, 0x1EE32, WBP::ALetter},
    {0x1EE34, 0x1EE37, WBP::ALetter},
    {0x1EE39, 0x1EE39, WBP::ALetter},
    {0x1EE3B, 0x1EE3B, WBP::ALetter},
    {0x1EE42, 0x1EE42, WBP::ALetter},
    {0x1EE47, 0x1EE47, WBP::ALetter},
    {0x1EE49, 0x1EE49, WBP::ALetter},
    {0x1EE4B, 0x1EE4B, WBP::ALetter},
    {0x1EE4D, 0x1EE4F, WBP::ALetter},
    {0x1EE51, 0x1EE52, WBP::ALetter},
    {0x1EE54, 0x1EE54, WBP::ALetter},
    {0x1EE57, 0x1EE57, WBP::ALetter},
    {0x1EE59, 0x1EE59, WBP::ALetter},
    {0x1EE5B, 0x1EE5B, WBP::ALetter},
    {0x1EE5D, 0x1EE5D, WBP::ALetter},
    {0x1EE5F, 0x1EE5F, WBP::ALetter},
    {0x1EE61, 0x1EE62, WBP::ALetter},
    {0x1EE64, 0x1EE64, WBP::ALetter},
    {0x1EE67, 0x1EE6A, WBP::ALetter},
    {0x1EE6C, 0x1EE72, WBP::ALetter},
    {0x1EE74, 0x1EE77, WBP::ALetter},
    {0x1EE79, 0x1EE7C, WBP::ALetter},
    {0x1EE7E, 0x1EE7E, WBP::ALetter},
    {0x1EE80, 0x1EE89, WBP::ALetter},
    {0x1EE8B, 0x1EE9B, WBP::ALetter},
    {0x1EEA1, 0x1EEA3, WBP::ALetter},
    {0x1EEA5, 0x1EEA9, WBP::ALetter},
    {0x1EEAB, 0x1EEBB, WBP::ALetter},
    {0x1F130, 0x1F149, WBP::ALetter},
    {0x1F150, 0x1F169, WBP::ALetter},
    {0x1F170, 0x1F189, WBP::ALetter},
    {0x1F1E6, 0x1F1FF, WBP::Regional_Indicator},
    {0x1F3FB, 0x1F3FF, WBP::Extend},
    {0x1FBF0, 0x1FBF9, WBP::Numeric},
    {0xE0001, 0xE0001, WBP::Format},
    {0xE0020, 0xE007F, WBP::Extend},
    {0xE0100, 0xE01EF, WBP::Extend},
}};

// Construct table of just WBP::Extend character intervals
constexpr auto g_extend_characters{[]() constexpr {
  // Compute number of extend character intervals
  constexpr size_t size = []() constexpr {
    size_t count = 0;
    for (auto interval : g_word_break_intervals) {
      if (interval.property == WBP::Extend) {
        count++;
      }
    }
    return count;
  }();

  // Create array of extend character intervals
  std::array<Interval, size> result{};
  size_t index = 0;
  for (auto interval : g_word_break_intervals) {
    if (interval.property == WBP::Extend) {
      result[index++] = {interval.first, interval.last};  // NOLINT
    }
  }
  return result;
}()};

// Find a codepoint inside a sorted list of Interval.
template <size_t N>
bool Bisearch(uint32_t ucs, const std::array<Interval, N>& table) {
  if (ucs < table.front().first || ucs > table.back().last) {  // NOLINT
    return false;
  }

  int min = 0;
  int max = N - 1;
  while (max >= min) {
    const int mid = (min + max) / 2;
    if (ucs > table[mid].last) {  // NOLINT
      min = mid + 1;
    } else if (ucs < table[mid].first) {  // NOLINT
      max = mid - 1;
    } else {
      return true;
    }
  }

  return false;
}

// Find a value inside a sorted list of Interval + property.
template <class C, size_t N>
bool Bisearch(uint32_t ucs, const std::array<C, N>& table, C* out) {
  if (ucs < table.front().first || ucs > table.back().last) {  // NOLINT
    return false;
  }

  int min = 0;
  int max = N - 1;
  while (max >= min) {
    const int mid = (min + max) / 2;
    if (ucs > table[mid].last) {  // NOLINT
      min = mid + 1;
    } else if (ucs < table[mid].first) {  // NOLINT
      max = mid - 1;
    } else {
      *out = table[mid];  // NOLINT
      return true;
    }
  }

  return false;
}

int codepoint_width(uint32_t ucs) {
  if (ftxui::IsControl(ucs)) {
    return -1;
  }

  if (ftxui::IsCombining(ucs)) {
    return 0;
  }

  if (ftxui::IsFullWidth(ucs)) {
    return 2;
  }

  return 1;
}

}  // namespace

namespace ftxui {

// From UTF8 encoded string |input|, eat in between 1 and 4 byte representing
// one codepoint. Put the codepoint into |ucs|. Start at |start| and update
// |end| to represent the beginning of the next byte to eat for consecutive
// executions.
bool EatCodePoint(const std::string& input,
                  size_t start,
                  size_t* end,
                  uint32_t* ucs) {
  if (start >= input.size()) {
    *end = start + 1;
    return false;
  }
  const uint8_t C0 = input[start];

  // 1 byte string.
  if ((C0 & 0b1000'0000) == 0b0000'0000) {  // NOLINT
    *ucs = C0 & 0b0111'1111;                // NOLINT
    *end = start + 1;
    return true;
  }

  // 2 byte string.
  if ((C0 & 0b1110'0000) == 0b1100'0000 &&  // NOLINT
      start + 1 < input.size()) {
    const uint8_t C1 = input[start + 1];
    *ucs = 0;
    *ucs += C0 & 0b0001'1111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C1 & 0b0011'1111;  // NOLINT
    *end = start + 2;
    return true;
  }

  // 3 byte string.
  if ((C0 & 0b1111'0000) == 0b1110'0000 &&  // NOLINT
      start + 2 < input.size()) {
    const uint8_t C1 = input[start + 1];
    const uint8_t C2 = input[start + 2];
    *ucs = 0;
    *ucs += C0 & 0b0000'1111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C1 & 0b0011'1111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C2 & 0b0011'1111;  // NOLINT
    *end = start + 3;
    return true;
  }

  // 4 byte string.
  if ((C0 & 0b1111'1000) == 0b1111'0000 &&  // NOLINT
      start + 3 < input.size()) {
    const uint8_t C1 = input[start + 1];
    const uint8_t C2 = input[start + 2];
    const uint8_t C3 = input[start + 3];
    *ucs = 0;
    *ucs += C0 & 0b0000'0111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C1 & 0b0011'1111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C2 & 0b0011'1111;  // NOLINT
    *ucs <<= 6;                // NOLINT
    *ucs += C3 & 0b0011'1111;  // NOLINT
    *end = start + 4;
    return true;
  }

  *end = start + 1;
  return false;
}

// From UTF16 encoded string |input|, eat in between 1 and 4 byte representing
// one codepoint. Put the codepoint into |ucs|. Start at |start| and update
// |end| to represent the beginning of the next byte to eat for consecutive
// executions.
bool EatCodePoint(const std::wstring& input,
                  size_t start,
                  size_t* end,
                  uint32_t* ucs) {
  if (start >= input.size()) {
    *end = start + 1;
    return false;
  }

  // On linux wstring uses the UTF32 encoding:
  if constexpr (sizeof(wchar_t) == 4) {
    *ucs = input[start];  // NOLINT
    *end = start + 1;
    return true;
  }

  // On windows, wstring uses the UTF16 encoding:
  int32_t C0 = input[start];  // NOLINT

  // 1 word size:
  if (C0 < 0xd800 || C0 >= 0xdc00) {  // NOLINT
    *ucs = C0;
    *end = start + 1;
    return true;
  }

  // 2 word size:
  if (start + 1 >= input.size()) {
    *end = start + 2;
    return false;
  }

  int32_t C1 = input[start + 1];                         // NOLINT
  *ucs = ((C0 & 0x3ff) << 10) + (C1 & 0x3ff) + 0x10000;  // NOLINT
  *end = start + 2;
  return true;
}

bool IsCombining(uint32_t ucs) {
  return Bisearch(ucs, g_extend_characters);
}

bool IsFullWidth(uint32_t ucs) {
  if (ucs < 0x0300)  // Quick path: // NOLINT
    return false;

  return Bisearch(ucs, g_full_width_characters);
}

bool IsControl(uint32_t ucs) {
  if (ucs == 0) {
    return true;
  }
  if (ucs < 32) {  // NOLINT
    const uint32_t LINE_FEED = 10;
    return ucs != LINE_FEED;
  }
  if (ucs >= 0x7f && ucs < 0xa0) {  // NOLINT
    return true;
  }
  return false;
}

WordBreakProperty CodepointToWordBreakProperty(uint32_t codepoint) {
  WordBreakPropertyInterval interval = {0, 0, WBP::ALetter};
  std::ignore = Bisearch(codepoint, g_word_break_intervals, &interval);
  return interval.property;
}

int wchar_width(wchar_t ucs) {
  return codepoint_width(uint32_t(ucs));
}

int wstring_width(const std::wstring& text) {
  int width = 0;

  for (const wchar_t& it : text) {
    const int w = wchar_width(it);
    if (w < 0) {
      return -1;
    }
    width += w;
  }
  return width;
}

int string_width(const std::string& input) {
  int width = 0;
  size_t start = 0;
  while (start < input.size()) {
    uint32_t codepoint = 0;
    if (!EatCodePoint(input, start, &start, &codepoint)) {
      continue;
    }

    if (IsControl(codepoint)) {
      continue;
    }

    if (IsCombining(codepoint)) {
      continue;
    }

    if (IsFullWidth(codepoint)) {
      width += 2;
      continue;
    }

    width += 1;
  }
  return width;
}

std::vector<std::string> Utf8ToGlyphs(const std::string& input) {
  std::vector<std::string> out;
  out.reserve(input.size());
  size_t start = 0;
  size_t end = 0;
  while (start < input.size()) {
    uint32_t codepoint = 0;
    if (!EatCodePoint(input, start, &end, &codepoint)) {
      start = end;
      continue;
    }

    const std::string append = input.substr(start, end - start);
    start = end;

    // Ignore control characters.
    if (IsControl(codepoint)) {
      continue;
    }

    // Combining characters are put with the previous glyph they are modifying.
    if (IsCombining(codepoint)) {
      if (!out.empty()) {
        out.back() += append;
      }
      continue;
    }

    // Fullwidth characters take two cells. The second is made of the empty
    // string to reserve the space the first is taking.
    if (IsFullWidth(codepoint)) {
      out.push_back(append);
      out.emplace_back("");
      continue;
    }

    // Normal characters:
    out.push_back(append);
  }
  return out;
}

size_t GlyphPrevious(const std::string& input, size_t start) {
  while (true) {
    if (start == 0) {
      return 0;
    }
    start--;

    // Skip the UTF8 continuation bytes.
    if ((input[start] & 0b1100'0000) == 0b1000'0000) {
      continue;
    }

    uint32_t codepoint = 0;
    size_t end = 0;
    const bool eaten = EatCodePoint(input, start, &end, &codepoint);

    // Ignore invalid, control characters and combining characters.
    if (!eaten || IsControl(codepoint) || IsCombining(codepoint)) {
      continue;
    }

    return start;
  }
}

size_t GlyphNext(const std::string& input, size_t start) {
  bool glyph_found = false;
  while (start < input.size()) {
    size_t end = 0;
    uint32_t codepoint = 0;
    const bool eaten = EatCodePoint(input, start, &end, &codepoint);

    // Ignore invalid, control characters and combining characters.
    if (!eaten || IsControl(codepoint) || IsCombining(codepoint)) {
      start = end;
      continue;
    }

    // We eat the beginning of the next glyph. If we are eating the one
    // requested, return its start position immediately.
    if (glyph_found) {
      return static_cast<int>(start);
    }

    // Otherwise, skip this glyph and iterate:
    glyph_found = true;
    start = end;
  }
  return static_cast<int>(input.size());
}

size_t GlyphIterate(const std::string& input, int glyph_offset, size_t start) {
  if (glyph_offset >= 0) {
    for (int i = 0; i < glyph_offset; ++i) {
      start = GlyphNext(input, start);
    }
    return start;
  } else {
    for (int i = 0; i < -glyph_offset; ++i) {
      start = GlyphPrevious(input, start);
    }
    return start;
  }
}

std::vector<int> CellToGlyphIndex(const std::string& input) {
  int x = -1;
  std::vector<int> out;
  out.reserve(input.size());
  size_t start = 0;
  size_t end = 0;
  while (start < input.size()) {
    uint32_t codepoint = 0;
    const bool eaten = EatCodePoint(input, start, &end, &codepoint);
    start = end;

    // Ignore invalid / control characters.
    if (!eaten || IsControl(codepoint)) {
      continue;
    }

    // Combining characters are put with the previous glyph they are modifying.
    if (IsCombining(codepoint)) {
      if (x == -1) {
        ++x;
        out.push_back(x);
      }
      continue;
    }

    // Fullwidth characters take two cells. The second is made of the empty
    // string to reserve the space the first is taking.
    if (IsFullWidth(codepoint)) {
      ++x;
      out.push_back(x);
      out.push_back(x);
      continue;
    }

    // Normal characters:
    ++x;
    out.push_back(x);
  }
  return out;
}

int GlyphCount(const std::string& input) {
  int size = 0;
  size_t start = 0;
  size_t end = 0;
  while (start < input.size()) {
    uint32_t codepoint = 0;
    const bool eaten = EatCodePoint(input, start, &end, &codepoint);
    start = end;

    // Ignore invalid characters:
    if (!eaten || IsControl(codepoint)) {
      continue;
    }

    // Ignore combining characters, except when they don't have a preceding to
    // combine with.
    if (IsCombining(codepoint)) {
      if (size == 0) {
        size++;
      }
      continue;
    }

    size++;
  }
  return size;
}

std::vector<WordBreakProperty> Utf8ToWordBreakProperty(
    const std::string& input) {
  std::vector<WordBreakProperty> out;
  out.reserve(input.size());
  size_t start = 0;
  size_t end = 0;
  while (start < input.size()) {
    uint32_t codepoint = 0;
    if (!EatCodePoint(input, start, &end, &codepoint)) {
      start = end;
      continue;
    }
    start = end;

    // Ignore control characters.
    if (IsControl(codepoint)) {
      continue;
    }

    // Ignore combining characters.
    if (IsCombining(codepoint)) {
      continue;
    }

    WordBreakPropertyInterval interval = {0, 0, WBP::ALetter};
    std::ignore = Bisearch(codepoint, g_word_break_intervals, &interval);
    out.push_back(interval.property);
  }
  return out;
}

/// Convert a UTF8 std::string into a std::wstring.
std::string to_string(const std::wstring& s) {
  std::string out;

  size_t i = 0;
  uint32_t codepoint = 0;
  while (EatCodePoint(s, i, &i, &codepoint)) {
    // Code point <-> UTF-8 conversion
    //
    // ┏━━━━━━━━┳━━━━━━━━┳━━━━━━━━┳━━━━━━━━┓
    // ┃Byte 1  ┃Byte 2  ┃Byte 3  ┃Byte 4  ┃
    // ┡━━━━━━━━╇━━━━━━━━╇━━━━━━━━╇━━━━━━━━┩
    // │0xxxxxxx│        │        │        │
    // ├────────┼────────┼────────┼────────┤
    // │110xxxxx│10xxxxxx│        │        │
    // ├────────┼────────┼────────┼────────┤
    // │1110xxxx│10xxxxxx│10xxxxxx│        │
    // ├────────┼────────┼────────┼────────┤
    // │11110xxx│10xxxxxx│10xxxxxx│10xxxxxx│
    // └────────┴────────┴────────┴────────┘

    // 1 byte UTF8
    if (codepoint <= 0b000'0000'0111'1111) {  // NOLINT
      const uint8_t p1 = codepoint;
      out.push_back(p1);  // NOLINT
      continue;
    }

    // 2 bytes UTF8
    if (codepoint <= 0b000'0111'1111'1111) {  // NOLINT
      uint8_t p2 = codepoint & 0b111111;      // NOLINT
      codepoint >>= 6;                        // NOLINT
      uint8_t p1 = codepoint;                 // NOLINT
      out.push_back(0b11000000 + p1);         // NOLINT
      out.push_back(0b10000000 + p2);         // NOLINT
      continue;
    }

    // 3 bytes UTF8
    if (codepoint <= 0b1111'1111'1111'1111) {  // NOLINT
      uint8_t p3 = codepoint & 0b111111;       // NOLINT
      codepoint >>= 6;                         // NOLINT
      uint8_t p2 = codepoint & 0b111111;       // NOLINT
      codepoint >>= 6;                         // NOLINT
      uint8_t p1 = codepoint;                  // NOLINT
      out.push_back(0b11100000 + p1);          // NOLINT
      out.push_back(0b10000000 + p2);          // NOLINT
      out.push_back(0b10000000 + p3);          // NOLINT
      continue;
    }

    // 4 bytes UTF8
    if (codepoint <= 0b1'0000'1111'1111'1111'1111) {  // NOLINT
      uint8_t p4 = codepoint & 0b111111;              // NOLINT
      codepoint >>= 6;                                // NOLINT
      uint8_t p3 = codepoint & 0b111111;              // NOLINT
      codepoint >>= 6;                                // NOLINT
      uint8_t p2 = codepoint & 0b111111;              // NOLINT
      codepoint >>= 6;                                // NOLINT
      uint8_t p1 = codepoint;                         // NOLINT
      out.push_back(0b11110000 + p1);                 // NOLINT
      out.push_back(0b10000000 + p2);                 // NOLINT
      out.push_back(0b10000000 + p3);                 // NOLINT
      out.push_back(0b10000000 + p4);                 // NOLINT
      continue;
    }

    // Something else?
  }
  return out;
}

/// Convert a std::wstring into a UTF8 std::string.
std::wstring to_wstring(const std::string& s) {
  std::wstring out;

  size_t i = 0;
  uint32_t codepoint = 0;
  while (EatCodePoint(s, i, &i, &codepoint)) {
    // On linux wstring are UTF32 encoded:
    if constexpr (sizeof(wchar_t) == 4) {
      out.push_back(codepoint);  // NOLINT
      continue;
    }

    // On Windows, wstring are UTF16 encoded:

    // Codepoint encoded using 1 word:
    // NOLINTNEXTLINE
    if (codepoint < 0xD800 || (codepoint > 0xDFFF && codepoint < 0x10000)) {
      uint16_t p0 = codepoint;  // NOLINT
      out.push_back(p0);        // NOLINT
      continue;
    }

    // Codepoint encoded using 2 words:
    codepoint -= 0x010000;                               // NOLINT
    uint16_t p0 = (((codepoint << 12) >> 22) + 0xD800);  // NOLINT
    uint16_t p1 = (((codepoint << 22) >> 22) + 0xDC00);  // NOLINT
    out.push_back(p0);                                   // NOLINT
    out.push_back(p1);                                   // NOLINT
  }
  return out;
}

}  // namespace ftxui
