#include "ftxui/component/event.hpp"

namespace ftxui {

constexpr int ESC = int(27);

// --- Character ---
Event Event::Character(int c) {
  return Event{c};
}

// --- Arrow ---
Event Event::ArrowLeft{ESC, '[', 'D'};
Event Event::ArrowRight{ESC, '[', 'C'};
Event Event::ArrowUp{ESC, '[', 'A'};
Event Event::ArrowDown{ESC, '[', 'B'};

// --- Other ---
Event Event::Backspace{127};
Event Event::Delete{ESC, '[', '3', '~'};
Event Event::Escape{ESC};
Event Event::Return{10};

Event Event::F1{ESC, '[', 'O', 'P'};
Event Event::F2{ESC, '[', 'O', 'Q'};
Event Event::F3{ESC, '[', 'O', 'R'};
Event Event::F4{ESC, '[', 'O', 'S'};
Event Event::F5{ESC, '[', '1', '5', '~'};
Event Event::F6{ESC, '[', '1', '7', '~'};
Event Event::F7{ESC, '[', '1', '8', '~'};
Event Event::F8{ESC, '[', '1', '9', '~'};
Event Event::F9{ESC, '[', '2', '0', '~'};
Event Event::F10{ESC, '[', '2', '1', '~'};
Event Event::F11{ESC, '[', '2', '1', '~'};  // Same as F10 ?
Event Event::F12{ESC, '[', '2', '4', '~'};

} // namespace ftxui
