// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/terminal_id.hpp"

namespace ftxui
{

std::ostream& operator<<(
	std::ostream& os,
	TerminalID terminal_id)
{
	switch(terminal_id)
	{
		case TerminalID::UNKNOWN:
		{
			os << "UNKNOWN";
			break;
		}
		case TerminalID::XTERM:
		{
			os << "XTERM";
			break;
		}
		case TerminalID::KONSOLE:
		{
			os << "KONSOLE";
			break;
		}
		case TerminalID::URXVT:
		{
			os << "URXVT";
			break;
		}
		case TerminalID::VTE:
		{
			os << "VTE";
			break;
		}
		case TerminalID::LINUXVC:
		{
			os << "LINUXVC";
			break;
		}
	}

	return os;
}

}  // namespace ftxui
