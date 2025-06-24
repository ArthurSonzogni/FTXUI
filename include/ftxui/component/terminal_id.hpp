// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_COMPONENT_TERMINAL_ID_HPP
#define FTXUI_COMPONENT_TERMINAL_ID_HPP

#include <ostream>
#include <string>

namespace ftxui {

/// @brief The TerminalID enum class represents different types of terminal
/// emulators that FTXUI can detect. It is used to identify the terminal
/// emulator in use, which can affect how FTXUI renders its output and handles
/// input events.
/// @ingroup component
enum class TerminalID {
  Unknown,
  // --

  Konsole,
  LinuxVC,
  Urxvt,
  Vte,
  Xterm,
};

std::ostream& operator<<(
	std::ostream& os,
	TerminalID terminal_id);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TERMINAL_ID_HPP */
