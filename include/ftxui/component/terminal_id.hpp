// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#ifndef FTXUI_COMPONENT_TERMINAL_ID_HPP
#define FTXUI_COMPONENT_TERMINAL_ID_HPP

#include <ostream>
#include <string>

namespace ftxui {

std::string const TERMINAL_ID_REQUEST("\x1b[0c");

/// @brief A mouse event. It contains the coordinate of the mouse, the button
/// pressed and the modifier (shift, ctrl, meta).
/// @ingroup component

enum class TerminalID
{
	UNKNOWN,
	XTERM,
	KONSOLE,
	URXVT,
	VTE,
	LINUXVC
};

std::ostream& operator<<(
	std::ostream& os,
	TerminalID terminal_id);

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TERMINAL_ID_HPP */
