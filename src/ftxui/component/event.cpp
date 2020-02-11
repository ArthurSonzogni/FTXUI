#include <iostream>
#include "ftxui/component/event.hpp"
#include "ftxui/screen/string.hpp"

namespace ftxui {

// static
Event Event::Character(const std::string& input) {
  Event event;
  event.input_ = input;
  event.is_character_ = true;
  event.character_ = to_wstring(input)[0];
  return event;
}

// static
Event Event::Character(char c) {
  return Character(wchar_t(c));
}

// static
Event Event::Character(wchar_t c) {
  Event event;
  event.input_ = {(char)c};
  event.is_character_ = true;
  event.character_ = c;
  return event;
}

// static
Event Event::Special(const std::string& input) {
  Event event;
  event.input_ = std::move(input);
  return event;
}

Event ParseUTF8(std::function<char()>& getchar, std::string& input) {
  if ((input[0] & 0b11000000) == 0b11000000)
    input += getchar();
  if ((input[0] & 0b11100000) == 0b11100000)
    input += getchar();
  if ((input[0] & 0b11110000) == 0b11110000)
    input += getchar();
  return Event::Character(input);
}

Event ParseCSI(std::function<char()> getchar, std::string& input) {
  while (1) {
    char c = getchar();
    input += c;

    if (c >= '0' && c<= '9')
      continue;

    if (c == ';')
      continue;

    if (c >= ' ' && c <= '~')
      return Event::Special(input);

    // Invalid ESC in CSI.
    if (c == '\x1B')
      return Event::Special(input);
  }
}

Event ParseDCS(std::function<char()> getchar, std::string& input) {
  // Parse until the string terminator ST.
  while (1) {
    input += getchar();
    if (input.back() != '\x1B')
      continue;
    input += getchar();
    if (input.back() != '\\')
      continue;
    return Event::Special(input);
  }
}

Event ParseOSC(std::function<char()> getchar, std::string& input) {
  // Parse until the string terminator ST.
  while (1) {
    input += getchar();
    if (input.back() != '\x1B')
      continue;
    input += getchar();
    if (input.back() != '\\')
      continue;
    return Event::Special(input);
  }
}

Event ParseESC(std::function<char()> getchar, std::string& input) {
  input += getchar();
  switch (input.back()) {
    case 'P': return ParseDCS(getchar, input);
    case '[': return ParseCSI(getchar, input);
    case ']': return ParseOSC(getchar, input);
    default:
      input += getchar();
      return Event::Special(input);
  }
}

// static
Event Event::GetEvent(std::function<char()> getchar) {
  std::string input;
  input += getchar();

  unsigned char head = input[0];
  switch (head) {
    case 24:           // CAN
    case 26:           // SUB
      return Event();  // Ignored.

    case 'P':
      return ParseDCS(getchar, input);

    case '\x1B':
      return ParseESC(getchar, input);
  }

  if (head < 32)  // C0
    return Event::Special(input);

  if (head == 127)  // Delete
    return Event::Special(input);

  return ParseUTF8(getchar, input);
}

// --- Arrow ---
Event Event::ArrowLeft = Event::Special("\x1B[D");
Event Event::ArrowRight = Event::Special("\x1B[C");
Event Event::ArrowUp = Event::Special("\x1B[A");
Event Event::ArrowDown = Event::Special("\x1B[B");
Event Event::Backspace = Event::Special({127});
Event Event::Delete = Event::Special("\x1B[3~");
Event Event::Escape = Event::Special("\x1B");
Event Event::Return = Event::Special({10});
Event Event::Tab = Event::Special({9});
Event Event::TabReverse = Event::Special({27, 91, 90});
Event Event::F1 = Event::Special("\x1B[OP");
Event Event::F2 = Event::Special("\x1B[OQ");
Event Event::F3 = Event::Special("\x1B[OR");
Event Event::F4 = Event::Special("\x1B[OS");
Event Event::F5 = Event::Special("\x1B[15~");
Event Event::F6 = Event::Special("\x1B[17~");
Event Event::F7 = Event::Special("\x1B[18~");
Event Event::F8 = Event::Special("\x1B[19~");
Event Event::F9 = Event::Special("\x1B[20~");
Event Event::F10 = Event::Special("\x1B[21~");
Event Event::F11 = Event::Special("\x1B[21~");  // Doesn't exist
Event Event::F12 = Event::Special("\x1B[24~");
Event Event::Custom = Event::Special({0});

}  // namespace ftxui
