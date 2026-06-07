// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cstddef>
#include <string>
#include <string_view>
#include "ftxui/screen/string.hpp"

using namespace ftxui;

extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size) {
  std::string_view input(data, size);

  // Fuzz to_wstring
  try {
    to_wstring(input);
  } catch (...) {
  }

  // Fuzz string_width
  try {
    string_width(input);
  } catch (...) {
  }

  // Fuzz Utf8ToGlyphs
  try {
    Utf8ToGlyphs(input);
  } catch (...) {
  }

  // Fuzz CellToGlyphIndex
  try {
    CellToGlyphIndex(input);
  } catch (...) {
  }

  return 0;
}
