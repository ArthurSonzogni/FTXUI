#include "ftxui/component/terminal_input_parser.hpp"

#include <algorithm>  // for max
#include <cstdint>
#include <memory>   // for unique_ptr
#include <utility>  // for move

#include "ftxui/component/event.hpp"  // for Event

namespace ftxui {

TerminalInputParser::TerminalInputParser(Sender<Event> out)
    : out_(std::move(out)) {}

void TerminalInputParser::Timeout(int time) {
  timeout_ += time;
  if (timeout_ < 50)
    return;
  timeout_ = 0;
  if (pending_.size())
    Send(SPECIAL);
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
  return position_ < (int)pending_.size();
}

void TerminalInputParser::Send(TerminalInputParser::Output output) {
  switch (output.type) {
    case UNCOMPLETED:
      return;

    case DROP:
      pending_.clear();
      return;

    case CHARACTER:
      out_->Send(Event::Character(std::move(pending_)));
      pending_.clear();
      return;

    case SPECIAL:
      out_->Send(Event::Special(std::move(pending_)));
      pending_.clear();
      return;

    case MOUSE:
      out_->Send(Event::Mouse(std::move(pending_), output.mouse));
      pending_.clear();
      return;

    case CURSOR_REPORTING:
      out_->Send(Event::CursorReporting(std::move(pending_), output.cursor.x,
                                        output.cursor.y));
      pending_.clear();
      return;
  }
  // NOT_REACHED().
}

TerminalInputParser::Output TerminalInputParser::Parse() {
  if (!Eat())
    return UNCOMPLETED;

  switch (Current()) {
    case 24:  // CAN
    case 26:  // SUB
      return DROP;

    case '\x1B':
      return ParseESC();
    default:
      break;
  }

  if (Current() < 32)  // C0
    return SPECIAL;

  if (Current() == 127)  // Delete
    return SPECIAL;

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
  unsigned char head = static_cast<unsigned char>(Current());
  unsigned char selector = 0b1000'0000;

  // The non code-point part of the first byte.
  unsigned char mask = selector;

  // Find the first zero in the first byte.
  int first_zero = 8;
  for (int i = 0; i < 8; ++i) {
    mask |= selector;
    if (head & selector) {
      selector >>= 1;
      continue;
    }
    first_zero = i;
    break;
  }

  // Accumulate the value of the first byte.
  uint32_t value = head & ~mask;

  // Invalid UTF8, with more than 5 bytes.
  if (first_zero == 1 || first_zero >= 5)
    return DROP;

  // Multi byte UTF-8.
  for (int i = 2; i <= first_zero; ++i) {
    if (!Eat())
      return UNCOMPLETED;

    // Invalid continuation byte.
    head = static_cast<unsigned char>(Current());
    if ((head & 0b1100'0000) != 0b1000'0000)
      return DROP;
    value <<= 6;
    value += head & 0b0011'1111;
  }

  // Check for overlong UTF8 encoding.
  int extra_byte;
  if (value <= 0b000'0000'0111'1111) {
    extra_byte = 0;
  } else if (value <= 0b000'0111'1111'1111) {
    extra_byte = 1;
  } else if (value <= 0b1111'1111'1111'1111) {
    extra_byte = 2;
  } else if (value <= 0b1'0000'1111'1111'1111'1111) {
    extra_byte = 3;
  } else {
    return DROP;
  }

  if (extra_byte != position_)
    return DROP;

  return CHARACTER;
}

TerminalInputParser::Output TerminalInputParser::ParseESC() {
  if (!Eat())
    return UNCOMPLETED;
  switch (Current()) {
    case 'P':
      return ParseDCS();
    case '[':
      return ParseCSI();
    case ']':
      return ParseOSC();
    default:
      if (!Eat())
        return UNCOMPLETED;
      return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseDCS() {
  // Parse until the string terminator ST.
  while (1) {
    if (!Eat())
      return UNCOMPLETED;

    if (Current() != '\x1B')
      continue;

    if (!Eat())
      return UNCOMPLETED;

    if (Current() != '\\')
      continue;

    return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseCSI() {
  bool altered = false;
  int argument = 0;
  std::vector<int> arguments;
  while (true) {
    if (!Eat())
      return UNCOMPLETED;

    if (Current() == '<') {
      altered = true;
      continue;
    }

    if (Current() >= '0' && Current() <= '9') {
      argument *= 10;
      argument += int(Current() - '0');
      continue;
    }

    if (Current() == ';') {
      arguments.push_back(argument);
      argument = 0;
      continue;
    }

    if (Current() >= ' ' && Current() <= '~' && Current() != '<') {
      arguments.push_back(argument);
      argument = 0;
      switch (Current()) {
        case 'M':
          return ParseMouse(altered, true, std::move(arguments));
        case 'm':
          return ParseMouse(altered, false, std::move(arguments));
        case 'R':
          return ParseCursorReporting(std::move(arguments));
        default:
          return SPECIAL;
      }
    }

    // Invalid ESC in CSI.
    if (Current() == '\x1B')
      return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseOSC() {
  // Parse until the string terminator ST.
  while (true) {
    if (!Eat())
      return UNCOMPLETED;
    if (Current() != '\x1B')
      continue;
    if (!Eat())
      return UNCOMPLETED;
    if (Current() != '\\')
      continue;
    return SPECIAL;
  }
}

TerminalInputParser::Output TerminalInputParser::ParseMouse(
    bool altered,
    bool pressed,
    std::vector<int> arguments) {
  if (arguments.size() != 3)
    return SPECIAL;

  (void)altered;

  Output output(MOUSE);
  output.mouse.button = Mouse::Button((arguments[0] & 3) +  //
                                      ((arguments[0] & 64) >> 4));
  output.mouse.motion = Mouse::Motion(pressed);
  output.mouse.shift = bool(arguments[0] & 4);
  output.mouse.meta = bool(arguments[0] & 8);
  output.mouse.x = arguments[1];
  output.mouse.y = arguments[2];
  return output;
}

TerminalInputParser::Output TerminalInputParser::ParseCursorReporting(
    std::vector<int> arguments) {
  if (arguments.size() != 2)
    return SPECIAL;
  Output output(CURSOR_REPORTING);
  output.cursor.y = arguments[0];
  output.cursor.x = arguments[1];
  return output;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
