// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_TERMINAL_HPP
#define FTXUI_SCREEN_TERMINAL_HPP

#include <string_view>
#include <vector>

namespace ftxui {

/// @brief Dimensions is a structure that represents the size of the terminal
/// @ingroup screen
struct Dimensions {
  int dimx;
  int dimy;
};

namespace Terminal {
Dimensions Size();
void SetFallbackSize(const Dimensions& fallbackSize);

/// @brief Color is an enumeration that represents the color support of the
/// terminal.
/// @ingroup screen
enum Color {
  Palette1,
  Palette16,
  Palette256,
  TrueColor,
};
Color ColorSupport();
void SetColorSupport(Color color);

/// @brief Quirks is a structure that represents various terminal-specific
/// behaviors that may require fallbacks.
/// @ingroup screen
struct Quirks {
  /// @brief Whether the terminal font supports the 8 Unicode block characters.
  bool block_characters = true;
  /// @brief Whether the terminal correctly handles hiding the cursor.
  bool cursor_hiding = true;
  /// @brief Whether the terminal should use ASCII characters for components.
  bool component_ascii = false;
  /// @brief The level of color support of the terminal.
  Color color_support = Palette256;
};
Quirks GetQuirks();
void SetQuirks(const Quirks& quirks);

/// @brief Compute the color support based on environment variables and terminal
/// identification.
/// @param term The TERM environment variable.
/// @param colorterm The COLORTERM environment variable.
/// @param term_program The TERM_PROGRAM environment variable.
/// @param terminal_name The terminal name (from DA2).
/// @param terminal_emulator_name The terminal emulator name (from XTVERSION).
/// @param capabilities The terminal capabilities (from DA1).
Color ComputeColorSupport(std::string_view term,
                          std::string_view colorterm,
                          std::string_view term_program,
                          std::string_view terminal_name,
                          std::string_view terminal_emulator_name,
                          const std::vector<int>& capabilities);

}  // namespace Terminal

}  // namespace ftxui

#endif  // FTXUI_SCREEN_TERMINAL_HPP
