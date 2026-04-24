// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/terminal_input_parser.hpp"

#include <cstdint>                    // for uint32_t
#include <ftxui/component/mouse.hpp>  // for Mouse, Mouse::Button, Mouse::Motion
#include <functional>                 // for std::function
#include <map>
#include <memory>   // for unique_ptr, allocator
#include <utility>  // for move
#include <vector>
#include "ftxui/component/event.hpp"  // for Event
#include "ftxui/component/task.hpp"   // for Task

namespace ftxui {

// NOLINTNEXTLINE
const std::map<std::string, std::string> g_uniformize = {
    // Microsoft's terminal uses a different new line character for the return
    // key. This also happens with linux with the `bind` command:
    // See https://github.com/ArthurSonzogni/FTXUI/issues/337
    // Here, we uniformize the new line character to `\n`.
    {"\r", "\n"},

    // See: https://github.com/ArthurSonzogni/FTXUI/issues/508
    {std::string({8}), std::string({127})},

    // See: https://github.com/ArthurSonzogni/FTXUI/issues/626
    //
    // Depending on the Cursor Key Mode (DECCKM), the terminal sends different
    // escape sequences:
    //
    //   Key     Normal    Application
    //   -----   --------  -----------
    //   Up      ESC [ A   ESC O A
    //   Down    ESC [ B   ESC O B
    //   Right   ESC [ C   ESC O C
    //   Left    ESC [ D   ESC O D
    //   Home    ESC [ H   ESC O H
    //   End     ESC [ F   ESC O F
    //
    {"\x1BOA", "\x1B[A"},  // UP
    {"\x1BOB", "\x1B[B"},  // DOWN
    {"\x1BOC", "\x1B[C"},  // RIGHT
    {"\x1BOD", "\x1B[D"},  // LEFT
    {"\x1BOH", "\x1B[H"},  // HOME
    {"\x1BOF", "\x1B[F"},  // END

    // Common Home/End sequences from terminals and multiplexers.
    {"\x1B[1~", "\x1B[H"},  // HOME
    {"\x1B[4~", "\x1B[F"},  // END

    // Variations around the FN keys.
    // Internally, we are using:
    // vt220, xterm-vt200, xterm-xf86-v44, xterm-new, mgt, screen
    // See: https://invisible-island.net/xterm/xterm-function-keys.html

    // For linux OS console (CTRL+ALT+FN), who do not belong to any
    // real standard.
    // See: https://github.com/ArthurSonzogni/FTXUI/issues/685
    {"\x1B[[A", "\x1BOP"},    // F1
    {"\x1B[[B", "\x1BOQ"},    // F2
    {"\x1B[[C", "\x1BOR"},    // F3
    {"\x1B[[D", "\x1BOS"},    // F4
    {"\x1B[[E", "\x1B[15~"},  // F5

    // xterm-r5, xterm-r6, rxvt
    {"\x1B[11~", "\x1BOP"},  // F1
    {"\x1B[12~", "\x1BOQ"},  // F2
    {"\x1B[13~", "\x1BOR"},  // F3
    {"\x1B[14~", "\x1BOS"},  // F4

    // vt100
    {"\x1BOt", "\x1B[15~"},  // F5
    {"\x1BOu", "\x1B[17~"},  // F6
    {"\x1BOv", "\x1B[18~"},  // F7
    {"\x1BOl", "\x1B[19~"},  // F8
    {"\x1BOw", "\x1B[20~"},  // F9
    {"\x1BOx", "\x1B[21~"},  // F10

    // scoansi
    {"\x1B[M", "\x1BOP"},    // F1
    {"\x1B[N", "\x1BOQ"},    // F2
    {"\x1B[O", "\x1BOR"},    // F3
    {"\x1B[P", "\x1BOS"},    // F4
    {"\x1B[Q", "\x1B[15~"},  // F5
    {"\x1B[R", "\x1B[17~"},  // F6
    {"\x1B[S", "\x1B[18~"},  // F7
    {"\x1B[T", "\x1B[19~"},  // F8
    {"\x1B[U", "\x1B[20~"},  // F9
    {"\x1B[V", "\x1B[21~"},  // F10
    {"\x1B[W", "\x1B[23~"},  // F11
    {"\x1B[X", "\x1B[24~"},  // F12
};

TerminalInputParser::TerminalInputParser(std::function<void(Event)> out)
    : out_(std::move(out)) {}

void TerminalInputParser::Timeout(int time) {
  timeout_ += time;
  const int timeout_threshold = 50;
  if (timeout_ < timeout_threshold) {
    return;
  }
  timeout_ = 0;
  if (!pending_.empty()) {
    Send(SPECIAL);
  }
}

void TerminalInputParser::Add(char c) {
  pending_ += c;
  timeout_ = 0;
  position_ = -1;
  Send(Parse());
}

unsigned char TerminalInputParser::Current() {
  return pending_[position_];
}

bool TerminalInputParser::Eat() {
  position_++;
  return position_ < static_cast<int>(pending_.size());
}

void TerminalInputParser::Send(TerminalInputParser::Output output) {
  switch (output.type) {
    case UNCOMPLETED:
      return;

    case DROP:
      pending_.clear();
      return;

    case CHARACTER:
      out_(Event::Character(std::move(pending_)));
      pending_.clear();
      return;

    case SPECIAL: {
      auto it = g_uniformize.find(pending_);
      if (it != g_uniformize.end()) {
        pending_ = it->second;
      }
      out_(Event::Special(std::move(pending_)));
      pending_.clear();
    }
      return;

    case MOUSE:
      out_(Event::Mouse(std::move(pending_), output.mouse));  // NOLINT
      pending_.clear();
      return;

    case CURSOR_POSITION:
      out_(Event::CursorPosition(std::move(pending_),  // NOLINT
                                 output.cursor.x,      // NOLINT
                                 output.cursor.y));    // NOLINT
      pending_.clear();
      return;

    case CURSOR_SHAPE:
      out_(Event::CursorShape(std::move(pending_), output.cursor_shape));
      pending_.clear();
      return;

    case TERMINAL_NAME_VERSION:
      out_(Event::TerminalNameVersion(std::move(pending_),
                                      std::move(output.terminal_name),
                                      output.terminal_version));
      pending_.clear();
      return;

    case TERMINAL_EMULATOR:
      out_(Event::TerminalEmulator(std::move(pending_),
                                   std::move(output.terminal_name),
                                   std::move(output.terminal_version_string)));
      pending_.clear();
      return;

    case TERMINAL_CAPABILITIES:
      out_(Event::TerminalCapabilities(
          std::move(pending_), std::move(output.terminal_capabilities)));
      pending_.clear();
      return;
  }
  // NOT_REACHED().
}

TerminalInputParser::Output TerminalInputParser::Parse() {
  if (!Eat()) {
    return UNCOMPLETED;
  }

  if (Current() == '\x1B') {
    return ParseESC();
  }

  if (Current() < 32) {  // C0 NOLINT
    return SPECIAL;
  }

  if (Current() == 127) {  // Delete // NOLINT
    return SPECIAL;
  }

  return ParseUTF8();
}

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
//
// Then some sequences are illegal if it exist a shorter representation of the
// same codepoint.
TerminalInputParser::Output TerminalInputParser::ParseUTF8() {
  auto head = Current();
  unsigned char selector = 0b1000'0000;  // NOLINT

  // The non code-point part of the first byte.
  unsigned char mask = selector;

  // Find the first zero in the first byte.
  unsigned int first_zero = 8;            // NOLINT
  for (unsigned int i = 0; i < 8; ++i) {  // NOLINT
    mask |= selector;
    if (!(head & selector)) {
      first_zero = i;
      break;
    }
    selector >>= 1U;
  }

  // Accumulate the value of the first byte.
  auto value = uint32_t(head & ~mask);  // NOLINT

  // Invalid UTF8, with more than 5 bytes.
  const unsigned int max_utf8_bytes = 5;
  if (first_zero == 1 || first_zero >= max_utf8_bytes) {
    return DROP;
  }

  // Multi byte UTF-8.
  for (unsigned int i = 2; i <= first_zero; ++i) {
    if (!Eat()) {
      return UNCOMPLETED;
    }

    // Invalid continuation byte.
    head = Current();
    if ((head & 0b1100'0000) != 0b1000'0000) {  // NOLINT
      return DROP;
    }
    value <<= 6;                  // NOLINT
    value += head & 0b0011'1111;  // NOLINT
  }

  // Check for overlong UTF8 encoding.
  int extra_byte = 0;
  if (value <= 0b000'0000'0111'1111) {                 // NOLINT
    extra_byte = 0;                                    // NOLINT
  } else if (value <= 0b000'0111'1111'1111) {          // NOLINT
    extra_byte = 1;                                    // NOLINT
  } else if (value <= 0b1111'1111'1111'1111) {         // NOLINT
    extra_byte = 2;                                    // NOLINT
  } else if (value <= 0b1'0000'1111'1111'1111'1111) {  // NOLINT
    extra_byte = 3;                                    // NOLINT
  } else {                                             // NOLINT
    return DROP;
  }

  if (extra_byte != position_) {
    return DROP;
  }

  return CHARACTER;
}

TerminalInputParser::Output TerminalInputParser::ParseESC() {
  if (!Eat()) {
    return UNCOMPLETED;
  }
  switch (Current()) {
    case 'P':
      return ParseDCS();
    case '[':
      return ParseCSI();
    case ']':
      return ParseOSC();

    // Expecting 2 characters.
    case ' ':
    case '#':
    case '%':
    case '(':
    case ')':
    case '*':
    case '+':
    case 'O':
    case 'N': {
      if (!Eat()) {
        return UNCOMPLETED;
      }
      return SPECIAL;
    }
    // Expecting 1 character:
    default:
      return SPECIAL;
  }
}

// ESC P ... ESC BACKSLASH
TerminalInputParser::Output TerminalInputParser::ParseDCS() {
  // Parse until the string terminator ST.
  while (true) {
    if (!Eat()) {
      return UNCOMPLETED;
    }

    if (Current() != '\x1B') {
      continue;
    }

    if (!Eat()) {
      return UNCOMPLETED;
    }

    if (Current() != '\\') {
      continue;
    }

    // XTVERSION: ESC P > | name version ST
    if (pending_.size() >= 5 && pending_[2] == '>' && pending_[3] == '|') {
      // ESC P > | name (version) ST
      // 0   1 2 3 4
      const std::string content = pending_.substr(4, pending_.size() - 6);
      Output output(TERMINAL_EMULATOR);
      const size_t space = content.find(' ');
      const size_t open_paren = content.find('(');
      if (space != std::string::npos) {
        output.terminal_name = content.substr(0, space);
        output.terminal_version_string = content.substr(space + 1);
      } else if (open_paren != std::string::npos) {
        output.terminal_name = content.substr(0, open_paren);
        output.terminal_version_string = content.substr(open_paren + 1);
        if (!output.terminal_version_string.empty() &&
            output.terminal_version_string.back() == ')') {
          output.terminal_version_string.pop_back();
        }
      } else {
        output.terminal_name = content;
        output.terminal_version_string = "unknown";
      }
      return output;
    }

    if (pending_.size() == 10 &&  //
        pending_[2] == '1' &&     //
        pending_[3] == '$' &&     //
        pending_[4] == 'r' &&     //
        true) {
      Output output(CURSOR_SHAPE);
      output.cursor_shape = pending_[5] - '0';
      return output;
    }

    return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseCSI() {
  bool altered_less = false;
  bool altered_greater = false;
  bool altered_question = false;
  int argument = 0;
  std::vector<int> arguments;
  while (true) {
    if (!Eat()) {
      return UNCOMPLETED;
    }

    if (Current() == '<') {
      altered_less = true;
      continue;
    }

    if (Current() == '>') {
      altered_greater = true;
      continue;
    }

    if (Current() == '?') {
      altered_question = true;
      continue;
    }

    if (Current() >= '0' && Current() <= '9') {
      argument *= 10;  // NOLINT
      argument += Current() - '0';
      continue;
    }

    if (Current() == ';') {
      arguments.push_back(argument);
      argument = 0;
      continue;
    }

    // CSI is terminated by a character in the range 0x40–0x7E
    // (ASCII @A–Z[\]^_`a–z{|}~),
    if (Current() >= '@' && Current() <= '~' &&
        // Note: I don't remember why we exclude '<'
        Current() != '<' &&
        // To handle F1-F4, we exclude '['.
        Current() != '[') {
      arguments.push_back(argument);
      argument = 0;  // NOLINT

      switch (Current()) {
        case 'M':
          return ParseMouse(altered_less, true, std::move(arguments));
        case 'm':
          return ParseMouse(altered_less, false, std::move(arguments));
        case 'R':
          return ParseCursorPosition(std::move(arguments));
        case 'c':
          return ParseDeviceAttributes(altered_greater, altered_question,
                                       std::move(arguments));
        default:
          return SPECIAL;
      }
    }

    // Invalid ESC in CSI.
    if (Current() == '\x1B') {
      return SPECIAL;
    }
  }
}

TerminalInputParser::Output TerminalInputParser::ParseOSC() {
  // Parse until the string terminator ST.
  while (true) {
    if (!Eat()) {
      return UNCOMPLETED;
    }
    if (Current() != '\x1B') {
      continue;
    }
    if (!Eat()) {
      return UNCOMPLETED;
    }
    if (Current() != '\\') {
      continue;
    }
    return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseMouse(  // NOLINT
    bool altered,
    bool pressed,
    std::vector<int> arguments) {
  if (arguments.size() != 3) {
    return SPECIAL;
  }

  (void)altered;

  Output output(MOUSE);
  output.mouse.motion = Mouse::Motion(pressed);  // NOLINT

  // Bits value Modifier  Comment
  // ---- ----- ------- ---------
  // 0 1  1 2   button   0 = Left, 1 = Middle, 2 = Right, 3 = Release
  // 2    4     Shift
  // 3    8     Meta
  // 4    16    Control
  // 5    32    Move
  // 6    64    Wheel

  // clang-format off
  const int button      = arguments[0] & (1 + 2); // NOLINT
  const bool is_shift   = arguments[0] & 4;       // NOLINT
  const bool is_meta    = arguments[0] & 8;       // NOLINT
  const bool is_control = arguments[0] & 16;      // NOLINT
  const bool is_move    = arguments[0] & 32;      // NOLINT
  const bool is_wheel   = arguments[0] & 64;      // NOLINT
  // clang-format on

  output.mouse.motion = is_move ? Mouse::Moved : Mouse::Motion(pressed);
  output.mouse.button = is_wheel ? Mouse::Button(Mouse::WheelUp + button)  //
                                 : Mouse::Button(button);
  output.mouse.shift = is_shift;
  output.mouse.meta = is_meta;
  output.mouse.control = is_control;
  output.mouse.x = arguments[1];  // NOLINT
  output.mouse.y = arguments[2];  // NOLINT

  // Motion event.
  return output;
}

// NOLINTNEXTLINE
TerminalInputParser::Output TerminalInputParser::ParseCursorPosition(
    std::vector<int> arguments) {
  if (arguments.size() != 2) {
    return SPECIAL;
  }
  Output output(CURSOR_POSITION);
  output.cursor.y = arguments[0];  // NOLINT
  output.cursor.x = arguments[1];  // NOLINT
  return output;
}

// NOLINTNEXTLINE
TerminalInputParser::Output TerminalInputParser::ParseDeviceAttributes(
    bool altered_greater,
    bool altered_question,
    std::vector<int> arguments) {
  if (altered_greater) {
    // Secondary Device Attributes (DA2)
    // ESC [ > Pp ; Pv ; Pc c
    if (arguments.size() >= 3) {
      // Pp: Terminal type
      // Pv: Firmware version
      // Pc: Hardware options
      Output output(TERMINAL_NAME_VERSION);
      output.terminal_version = arguments[1];
      switch (arguments[0]) {
        case 0:
          output.terminal_name = "xterm";
          break;
        case 1:
          output.terminal_name = "vt220";
          break;
        case 2:
          output.terminal_name = "vt240";
          break;
        case 18:
          output.terminal_name = "vt330";
          break;
        case 19:
          output.terminal_name = "vt340";
          break;
        case 24:
          output.terminal_name = "vt320";
          break;
        case 41:
          output.terminal_name = "vt420";
          break;
        case 61:
          output.terminal_name = "vt510";
          break;
        case 64:
          output.terminal_name = "vt520";
          break;
        case 65:
          output.terminal_name = "vt525";
          break;
        case 84:
          output.terminal_name = "tmux";
          break;
        case 85:
          output.terminal_name = "urxvt";
          break;
        default:
          output.terminal_name = "unknown";
          break;
      }
      // Special case for xterm which often returns 0;pv;0 or similar
      // but it's not strictly following DEC VT types.
      return output;
    }
  } else if (altered_question) {
    // Primary Device Attributes (DA1)
    // ESC [ ? Pp ; ... c
    Output output(TERMINAL_CAPABILITIES);
    output.terminal_capabilities = std::move(arguments);
    return output;
  }
  return SPECIAL;
}

}  // namespace ftxui
