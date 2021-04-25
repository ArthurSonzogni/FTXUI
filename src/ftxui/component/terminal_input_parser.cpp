#include "ftxui/component/terminal_input_parser.hpp"

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

TerminalInputParser::Output TerminalInputParser::ParseUTF8() {
  unsigned char head = static_cast<unsigned char>(Current());
  for (int i = 0; i < 3; ++i, head <<= 1) {
    if ((head & 0b11000000) != 0b11000000)
      break;
    if (!Eat())
      return UNCOMPLETED;
  }
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
