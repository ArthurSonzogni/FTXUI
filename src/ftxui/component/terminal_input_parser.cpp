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
      break;

    case CHARACTER:
      out_->Send(Event::Character(std::move(pending_)));
      break;

    case SPECIAL:
      out_->Send(Event::Special(std::move(pending_)));
      break;

    case MOUSE_MOVE:
      out_->Send(
          Event::MouseMove(std::move(pending_), output.mouse.x, output.mouse.y));
      break;

    case MOUSE_UP:
      out_->Send(
          Event::MouseUp(std::move(pending_), output.mouse.x, output.mouse.y));
      break;

    case MOUSE_LEFT_DOWN:
      out_->Send(Event::MouseLeftDown(std::move(pending_), output.mouse.x,
                                      output.mouse.y));
      break;

    case MOUSE_LEFT_MOVE:
      out_->Send(Event::MouseLeftMove(std::move(pending_), output.mouse.x,
                                  output.mouse.y));
      break;

    case MOUSE_RIGHT_DOWN:
      out_->Send(Event::MouseRightDown(std::move(pending_), output.mouse.x,
                                      output.mouse.y));
      break;

    case MOUSE_RIGHT_MOVE:
      out_->Send(Event::MouseRightMove(std::move(pending_), output.mouse.x,
                                  output.mouse.y));
      break;
  }
  pending_.clear();
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
  int argument;
  std::vector<int> arguments;
  while (true) {
    if (!Eat())
      return UNCOMPLETED;

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

    if (Current() >= ' ' && Current() <= '~') {
      arguments.push_back(argument);
      argument = 0;
      switch (Current()) {
        case 'M':
          return ParseMouse(std::move(arguments));
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
    std::vector<int> arguments) {
  if (arguments.size() != 3)
    return SPECIAL;
  switch(arguments[0]) {
    case 32:
      return Output(MOUSE_LEFT_DOWN, arguments[1], arguments[2]);
    case 64:
      return Output(MOUSE_LEFT_MOVE, arguments[1], arguments[2]);

    case 34:
      return Output(MOUSE_RIGHT_DOWN, arguments[1], arguments[2]);
    case 66:
      return Output(MOUSE_RIGHT_MOVE, arguments[1], arguments[2]);

    case 35:
      return Output(MOUSE_UP, arguments[1], arguments[2]);
    case 67:
      return Output(MOUSE_MOVE, arguments[1], arguments[2]);
  }
  return SPECIAL;
}

}  // namespace ftxui
