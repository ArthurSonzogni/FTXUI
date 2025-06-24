// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/terminal_id.hpp"

namespace ftxui
{

std::ostream& operator<<(std::ostream& os, TerminalID terminal_id) {
  switch (terminal_id) {
    case TerminalID::Unknown: {
      os << "Unknown";
      break;
    }
    case TerminalID::Urxvt: {
		  os << "Urxvt";
			break;
		}
		case TerminalID::LinuxVC: {
			os << "LinuxVC";
			break;
		}
		case TerminalID::Konsole: {
			os << "Konsole";
			break;
		}
		case TerminalID::Vte: {
			os << "Vte";
			break;
		}
		case TerminalID::Xterm: {
			os << "Xterm";
			break;
		}
  }

  return os;
}

}  // namespace ftxui
