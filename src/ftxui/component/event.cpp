#include "ftxui/component/event.hpp"

#include <iostream>

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

void ParseUTF8(Receiver<char>& in, Sender<Event>& out, std::string& input) {
  char c;
  unsigned char head = static_cast<unsigned char>(input[0]);
  for (int i = 0; i < 3; ++i, head <<= 1) {
    if ((head & 0b11000000) != 0b11000000)
      break;
    if (!in->Receive(&c))
      return;
    input += c;
  }
  out->Send(Event::Character(input));
}

void ParseCSI(Receiver<char>& in, Sender<Event>& out, std::string& input) {
  char c;
  while (1) {
    if (!in->Receive(&c))
      return;
    input += c;

    if (c >= '0' && c <= '9')
      continue;

    if (c == ';')
      continue;

    if (c >= ' ' && c <= '~')
      return out->Send(Event::Special(input));

    // Invalid ESC in CSI.
    if (c == '\x1B')
      return out->Send(Event::Special(input));
  }
}

void ParseDCS(Receiver<char>& in, Sender<Event>& out, std::string& input) {
  char c;
  // Parse until the string terminator ST.
  while (1) {
    if (!in->Receive(&c))
      return;
    input += c;
    if (input.back() != '\x1B')
      continue;
    if (!in->Receive(&c))
      return;
    input += c;
    if (input.back() != '\\')
      continue;
    return out->Send(Event::Special(input));
  }
}

void ParseOSC(Receiver<char>& in, Sender<Event>& out, std::string& input) {
  char c;
  // Parse until the string terminator ST.
  while (1) {
    if (!in->Receive(&c))
      return;
    input += c;
    if (input.back() != '\x1B')
      continue;
    if (!in->Receive(&c))
      return;
    input += c;
    if (input.back() != '\\')
      continue;
    return out->Send(Event::Special(input));
  }
}

void ParseESC(Receiver<char>& in, Sender<Event>& out, std::string& input) {
  char c;
  if (!in->Receive(&c))
    return;
  input += c;
  switch (c) {
    case 'P':
      return ParseDCS(in, out, input);
    case '[':
      return ParseCSI(in, out, input);
    case ']':
      return ParseOSC(in, out, input);
    default:
      if (!in->Receive(&c))
        return;
      input += c;
      out->Send(Event::Special(input));
  }
}

// static
void Event::Convert(Receiver<char>& in, Sender<Event>& out, char c) {
  std::string input;
  input += c;

  unsigned char head = input[0];
  switch (head) {
    case 24:  // CAN
    case 26:  // SUB
      return;

    case 'P':
      return ParseDCS(in, out, input);

    case '\x1B':
      return ParseESC(in, out, input);
  }

  if (head < 32)  // C0
    return out->Send(Event::Special(input));

  if (head == 127)  // Delete
    return out->Send(Event::Special(input));

  return ParseUTF8(in, out, input);
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
