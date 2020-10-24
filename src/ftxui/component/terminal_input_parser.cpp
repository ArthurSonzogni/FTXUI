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

void TerminalInputParser::Send(TerminalInputParser::Type type) {
  switch (type) {
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
  }
}

TerminalInputParser::Type TerminalInputParser::Parse() {
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

TerminalInputParser::Type TerminalInputParser::ParseUTF8() {
  unsigned char head = static_cast<unsigned char>(Current());
  for (int i = 0; i < 3; ++i, head <<= 1) {
    if ((head & 0b11000000) != 0b11000000)
      break;
    if (!Eat())
      return UNCOMPLETED;
  }
  return CHARACTER;
}

TerminalInputParser::Type TerminalInputParser::ParseESC() {
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

TerminalInputParser::Type TerminalInputParser::ParseDCS() {
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

TerminalInputParser::Type TerminalInputParser::ParseCSI() {
  while (true) {
    if (!Eat())
      return UNCOMPLETED;

    if (Current() >= '0' && Current() <= '9')
      continue;

    if (Current() == ';')
      continue;

    if (Current() >= ' ' && Current() <= '~')
      return SPECIAL;

    // Invalid ESC in CSI.
    if (Current() == '\x1B')
      return SPECIAL;
  }
}

TerminalInputParser::Type TerminalInputParser::ParseOSC() {
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
}  // namespace ftxui
