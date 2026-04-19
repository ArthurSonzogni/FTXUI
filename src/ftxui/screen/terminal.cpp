// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for std::search
#include <cctype>     // for std::tolower
#include <cstdlib>    // for getenv
#include <string>      // for string, allocator
#include <string_view> // for string_view

#include "ftxui/screen/terminal.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#else
#include <sys/ioctl.h>  // for winsize, ioctl, TIOCGWINSZ
#include <unistd.h>     // for STDOUT_FILENO
#endif

namespace ftxui {

namespace {

bool g_color_support_detected = false;
Terminal::Quirks g_quirks = [] {
  Terminal::Quirks quirks;
#if defined(_WIN32)
  quirks.block_characters = false;
  quirks.cursor_hiding = false;
  quirks.component_ascii = true;
#endif
  return quirks;
}();

Dimensions& FallbackSize() {
#if defined(__EMSCRIPTEN__)
  // This dimension was chosen arbitrarily to be able to display:
  // https://arthursonzogni.com/FTXUI/examples
  // This will have to be improved when someone has time to implement and need
  // it.
  constexpr int fallback_width = 140;
  constexpr int fallback_height = 43;
#else
  // The terminal size in VT100 was 80x24. It is still used nowadays by
  // default in many terminal emulator. That's a good choice for a fallback
  // value.
  constexpr int fallback_width = 80;
  constexpr int fallback_height = 24;
#endif
  static Dimensions g_fallback_size{
      fallback_width,
      fallback_height,
  };
  return g_fallback_size;
}

const char* Safe(const char* c) {
  return (c != nullptr) ? c : "";
}

bool Contains(std::string_view s, std::string_view key) {
  if (key.empty()) {
    return true;
  }
  auto it = std::search(
      s.begin(), s.end(), key.begin(), key.end(),
      [](char a, char b) {
        return std::tolower(static_cast<unsigned char>(a)) ==
               std::tolower(static_cast<unsigned char>(b));
      });
  return it != s.end();
}

bool ContainsAny(std::string_view s, std::initializer_list<std::string_view> keys) {
  for (std::string_view key : keys) {
    if (Contains(s, key)) {
      return true;
    }
  }
  return false;
}

Terminal::Color ComputeColorSupport() {
#if defined(__EMSCRIPTEN__)
  return Terminal::Color::TrueColor;
#endif

  std::string_view COLORTERM = Safe(std::getenv("COLORTERM"));  // NOLINT
  if (ContainsAny(COLORTERM, {"24bit", "truecolor"})) {
    return Terminal::Color::TrueColor;
  }

  std::string_view TERM_PROGRAM = Safe(std::getenv("TERM_PROGRAM"));  // NOLINT
  if (ContainsAny(TERM_PROGRAM, {"iterm", "apple_terminal", "vscode", "warp",
                                 "ghostty", "wezterm"})) {
    return Terminal::Color::TrueColor;
  }

  std::string_view TERM = Safe(std::getenv("TERM"));  // NOLINT
  if (ContainsAny(TERM, {"direct", "truecolor", "kitty", "alacritty", "foot"})) {
    return Terminal::Color::TrueColor;
  }

  if (Contains(TERM, "xterm") && !ContainsAny(TERM, {"rxvt", "urxvt"})) {
    return Terminal::Color::TrueColor;
  }

  if (ContainsAny(COLORTERM, {"256"}) ||
      ContainsAny(TERM, {"256", "xterm", "screen", "tmux"}) ||
      Contains(TERM_PROGRAM, "iterm")) {
    return Terminal::Color::Palette256;
  }

  return Terminal::Color::Palette16;
}

}  // namespace

namespace Terminal {

/// @brief Get the terminal size.
/// @return The terminal size.
/// @ingroup screen
Dimensions Size() {
#if defined(__EMSCRIPTEN__)
  // This dimension was chosen arbitrarily to be able to display:
  // https://arthursonzogni.com/FTXUI/examples
  // This will have to be improved when someone has time to implement and need
  // it.
  return FallbackSize();
#elif defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return Dimensions{csbi.srWindow.Right - csbi.srWindow.Left + 1,
                      csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
  }

  return FallbackSize();
#else
  winsize w{};
  const int status = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);  // NOLINT
  // The ioctl return value result should be checked. Some operating systems
  // don't support TIOCGWINSZ.
  if (w.ws_col == 0 || w.ws_row == 0 || status < 0) {
    return FallbackSize();
  }
  return Dimensions{w.ws_col, w.ws_row};
#endif
}

/// @brief Override terminal size in case auto-detection fails
/// @param fallbackSize Terminal dimensions to fallback to
void SetFallbackSize(const Dimensions& fallbackSize) {
  FallbackSize() = fallbackSize;
}

/// @brief Get the color support of the terminal.
/// @ingroup screen
Color ColorSupport() {
  if (!g_color_support_detected) {
    g_quirks.color_support = ComputeColorSupport();
    g_color_support_detected = true;
  }
  return g_quirks.color_support;
}

/// @brief Override terminal color support in case auto-detection fails
/// @ingroup dom
void SetColorSupport(Color color) {
  g_quirks.color_support = color;
  g_color_support_detected = true;
}

/// @brief Get the terminal quirks.
/// @ingroup screen
Quirks GetQuirks() {
  if (!g_color_support_detected) {
    g_quirks.color_support = ComputeColorSupport();
    g_color_support_detected = true;
  }
  return g_quirks;
}

/// @brief Override terminal quirks.
/// @ingroup screen
void SetQuirks(const Quirks& quirks) {
  g_quirks = quirks;
  g_color_support_detected = true;
}

}  // namespace Terminal
}  // namespace ftxui
