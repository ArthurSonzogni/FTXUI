// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_TERMINAL_HPP
#define FTXUI_SCREEN_TERMINAL_HPP

#include <memory>
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
class Quirks {
 public:
  Quirks();
  ~Quirks();
  Quirks(const Quirks&);
  Quirks& operator=(const Quirks&);
  Quirks(Quirks&&) noexcept;
  Quirks& operator=(Quirks&&) noexcept;

  /// @brief Whether the terminal font supports the 8 Unicode block characters.
  bool BlockCharacters() const;
  void SetBlockCharacters(bool v);

  /// @brief Whether the terminal correctly handles hiding the cursor.
  bool CursorHiding() const;
  void SetCursorHiding(bool v);

  /// @brief Whether the terminal should use ASCII characters for components.
  bool ComponentAscii() const;
  void SetComponentAscii(bool v);

  /// @brief The level of color support of the terminal.
  Color ColorSupport() const;
  void SetColorSupport(Color v);

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
Quirks GetQuirks();
void SetQuirks(const Quirks& quirks);

/// @brief TerminalInfo is a structure that contains information about the
/// terminal.
/// @ingroup screen
class TerminalInfo {
 public:
  TerminalInfo();
  ~TerminalInfo();
  TerminalInfo(const TerminalInfo&) = delete;
  TerminalInfo& operator=(const TerminalInfo&) = delete;
  TerminalInfo(TerminalInfo&&) noexcept;
  TerminalInfo& operator=(TerminalInfo&&) noexcept;

  void SetTerm(std::string_view term);
  void SetColorterm(std::string_view colorterm);
  void SetTermProgram(std::string_view term_program);
  void SetTerminalName(std::string_view terminal_name);
  void SetTerminalEmulatorName(std::string_view terminal_emulator_name);
  void SetCapabilities(std::vector<int> capabilities);

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
Color ComputeColorSupport(std::string_view term,
                          std::string_view colorterm,
                          std::string_view term_program,
                          std::string_view terminal_name,
                          std::string_view terminal_emulator_name,
                          const std::vector<int>& capabilities);

}  // namespace Terminal

}  // namespace ftxui

#endif  // FTXUI_SCREEN_TERMINAL_HPP
