// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_TERMINAL_HPP
#define FTXUI_SCREEN_TERMINAL_HPP

#include <cstdint>

#include <memory>
#include <string_view>
#include <vector>

#include "ftxui/util/export.hpp"

namespace ftxui {

/// @brief Dimensions is a structure that represents the size of the terminal
/// @ingroup screen
struct FTXUI_EXPORT(SCREEN) Dimensions {
  int dimx;
  int dimy;
};

namespace Terminal {
/// Query the current terminal size (uses fallback size if unavailable).
FTXUI_EXPORT(SCREEN) Dimensions Size();
/// Set the fallback size returned by Size() when not attached to a terminal.
FTXUI_EXPORT(SCREEN) void SetFallbackSize(const Dimensions& fallbackSize);

/// @brief Color is an enumeration that represents the color support of the
/// terminal.
/// @ingroup screen
enum Color : uint8_t {
  Palette1,
  Palette16,
  Palette256,
  TrueColor,
};
/// Return the detected (or overridden) terminal color capability.
FTXUI_EXPORT(SCREEN) Color ColorSupport();
/// Override the detected terminal color capability.
FTXUI_EXPORT(SCREEN) void SetColorSupport(Color color);

/// @brief Quirks is a structure that represents various terminal-specific
/// behaviors that may require fallbacks.
/// @ingroup screen
class FTXUI_EXPORT(SCREEN) Quirks {
 public:
  Quirks();
  ~Quirks();
  Quirks(const Quirks&);
  Quirks& operator=(const Quirks&);
  Quirks(Quirks&&) noexcept;
  Quirks& operator=(Quirks&&) noexcept;

  /// @brief Whether the terminal font supports the 8 Unicode block characters.
  bool BlockCharacters() const;
  /// Set whether the terminal supports Unicode block characters.
  void SetBlockCharacters(bool v);

  /// @brief Whether the terminal correctly handles hiding the cursor.
  bool CursorHiding() const;
  /// Set whether the terminal supports hiding the cursor.
  void SetCursorHiding(bool v);

  /// @brief Whether the terminal should use ASCII characters for components.
  bool ComponentAscii() const;
  /// Set whether components should use ASCII fallback characters.
  void SetComponentAscii(bool v);

  /// @brief The level of color support of the terminal.
  Color ColorSupport() const;
  /// Set the terminal color support level.
  void SetColorSupport(Color v);

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
/// Return the active terminal Quirks configuration.
FTXUI_EXPORT(SCREEN) Quirks GetQuirks();
/// Override the active terminal Quirks configuration.
FTXUI_EXPORT(SCREEN) void SetQuirks(const Quirks& quirks);

/// @brief TerminalInfo is a structure that contains information about the
/// terminal.
/// @ingroup screen
class FTXUI_EXPORT(SCREEN) TerminalInfo {
 public:
  TerminalInfo();
  ~TerminalInfo();
  TerminalInfo(const TerminalInfo&) = delete;
  TerminalInfo& operator=(const TerminalInfo&) = delete;
  TerminalInfo(TerminalInfo&&) noexcept;
  TerminalInfo& operator=(TerminalInfo&&) noexcept;

  /// Set the TERM environment variable value.
  void SetTerm(std::string_view term);
  /// Set the COLORTERM environment variable value.
  void SetColorterm(std::string_view colorterm);
  /// Set the TERM_PROGRAM environment variable value.
  void SetTermProgram(std::string_view term_program);
  /// Set the terminal name reported via DA2.
  void SetTerminalName(std::string_view terminal_name);
  /// Set the terminal emulator name reported via XTVERSION.
  void SetTerminalEmulatorName(std::string_view terminal_emulator_name);
  /// Set the terminal capability codes reported via DA1.
  void SetCapabilities(std::vector<int> capabilities);

  /// Compute the best Color support level from the stored terminal info.
  Color ComputeColorSupport() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

/// @brief Compute the color support based on environment variables and terminal
/// identification.
/// @param term The TERM environment variable.
/// @param colorterm The COLORTERM environment variable.
/// @param term_program The TERM_PROGRAM environment variable.
/// @param terminal_name The terminal name (from DA2).
/// @param terminal_emulator_name The terminal emulator name (from XTVERSION).
/// @param capabilities The terminal capabilities (from DA1).
FTXUI_EXPORT(SCREEN)
Color ComputeColorSupport(std::string_view term,
                          std::string_view colorterm,
                          std::string_view term_program,
                          std::string_view terminal_name,
                          std::string_view terminal_emulator_name,
                          const std::vector<int>& capabilities);

}  // namespace Terminal

}  // namespace ftxui

#endif  // FTXUI_SCREEN_TERMINAL_HPP
