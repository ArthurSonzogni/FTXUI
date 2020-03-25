#include "ftxui/component/screen_interactive.hpp"

#include <stdio.h>

#include <algorithm>
#include <csignal>
#include <iostream>
#include <stack>
#include <thread>

#include "ftxui/component/component.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
  #ifndef UNICODE
    #error Must be compiled in UNICODE mode
  #endif
#else
  #include <termios.h>
  #include <unistd.h>
#endif

// Quick exit is missing in standard CLang headers
#if defined(__clang__) && defined(__APPLE__)
  #define quick_exit(a) exit(a)
#endif

namespace ftxui {

// Produce a stream of Event from a stream of char.
void CharToEventStream(Receiver<char> receiver, Sender<Event> sender) {
  char c;
  while (receiver->Receive(&c))
    Event::Convert(receiver, sender, c);
}

#if defined(_WIN32)

void Win32EventListener(std::atomic<bool>* quit,
                        Sender<char> char_sender,
                        Sender<Event> event_sender) {
  auto console = GetStdHandle(STD_INPUT_HANDLE);
  while (!*quit) {
    // Throttle ReadConsoleInput by waiting 250ms, this wait function will
    // return if there is input in the console.
    auto wait_result = WaitForSingleObject(console, 250);
    if (wait_result == WAIT_TIMEOUT)
      continue;

    DWORD number_of_events = 0;
    if (!GetNumberOfConsoleInputEvents(console, &number_of_events))
      continue;
    if (number_of_events <= 0)
      continue;

    std::vector<INPUT_RECORD> records{number_of_events};
    DWORD number_of_events_read = 0;
    ReadConsoleInput(console, records.data(),
                     (DWORD)(records.size() * sizeof(INPUT_RECORD)),
                     &number_of_events_read);
    records.resize(number_of_events_read);

    for (const auto& r : records) {
      switch (r.EventType) {
        case KEY_EVENT: {
          auto key_event = r.Event.KeyEvent;
          // ignore UP key events
          if (key_event.bKeyDown == FALSE)
            continue;
          char_sender->Send((char)key_event.uChar.UnicodeChar);
        } break;
        case WINDOW_BUFFER_SIZE_EVENT:
          event_sender->Send(Event::Special({0}));
          break;
        case MENU_EVENT:
        case FOCUS_EVENT:
        case MOUSE_EVENT:
          // TODO(mauve): Implement later.
          break;
      }
    }
  }
}

#else

// Read char from the terminal.
void UnixEventListener(std::atomic<bool>* quit, Sender<char> sender) {
  // TODO(arthursonzogni): Use a timeout so that it doesn't block even if the
  // user doesn't generate new chars.
  while (!*quit)
    sender->Send((char)getchar());
}

#endif

static const char* HIDE_CURSOR = "\x1B[?25l";
static const char* SHOW_CURSOR = "\x1B[?25h";

static const char* DISABLE_LINE_WRAP = "\x1B[7l";
static const char* ENABLE_LINE_WRAP = "\x1B[7h";

using SignalHandler = void(int);
std::stack<std::function<void()>> on_exit_functions;
void OnExit(int signal) {
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }

  if (signal == SIGINT)
    quick_exit(SIGINT);
}

auto install_signal_handler = [](int sig, SignalHandler handler) {
  auto old_signal_handler = std::signal(sig, handler);
  on_exit_functions.push([&]() { std::signal(sig, old_signal_handler); });
};

std::function<void()> on_resize = [] {};
void OnResize(int /* signal */) {
  on_resize();
}

ScreenInteractive::ScreenInteractive(int dimx, int dimy, Dimension dimension)
    : Screen(dimx, dimy), dimension_(dimension) {
  event_receiver_ = MakeReceiver<Event>();
  event_sender_ = event_receiver_->MakeSender();
}

ScreenInteractive::~ScreenInteractive() {}

// static
ScreenInteractive ScreenInteractive::FixedSize(int dimx, int dimy) {
  return ScreenInteractive(dimx, dimy, Dimension::Fixed);
}

// static
ScreenInteractive ScreenInteractive::Fullscreen() {
  return ScreenInteractive(0, 0, Dimension::Fullscreen);
}

// static
ScreenInteractive ScreenInteractive::TerminalOutput() {
  return ScreenInteractive(0, 0, Dimension::TerminalOutput);
}

// static
ScreenInteractive ScreenInteractive::FitComponent() {
  return ScreenInteractive(0, 0, Dimension::FitComponent);
}

void ScreenInteractive::PostEvent(Event event) {
  if (!quit_)
    event_sender_->Send(event);
}

void ScreenInteractive::Loop(Component* component) {
  // Save the old terminal configuration and restore it on exit.
#if defined(_WIN32)
  // Enable VT processing on stdout and stdin
  auto stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  auto stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

  DWORD out_mode = 0;
  DWORD in_mode = 0;
  GetConsoleMode(stdout_handle, &out_mode);
  GetConsoleMode(stdin_handle, &in_mode);
  on_exit_functions.push([=] { SetConsoleMode(stdout_handle, out_mode); });
  on_exit_functions.push([=] { SetConsoleMode(stdin_handle, in_mode); });

  out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  out_mode |= DISABLE_NEWLINE_AUTO_RETURN;

  in_mode &= ~ENABLE_ECHO_INPUT;
  in_mode &= ~ENABLE_LINE_INPUT;
  in_mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
  in_mode |= ENABLE_WINDOW_INPUT;

  SetConsoleMode(stdin_handle, in_mode);
  SetConsoleMode(stdout_handle, out_mode);
#else
  struct termios terminal;
  tcgetattr(STDIN_FILENO, &terminal);
  on_exit_functions.push([=] { tcsetattr(STDIN_FILENO, TCSANOW, &terminal); });

  terminal.c_lflag &= ~ICANON;  // Non canonique terminal.
  terminal.c_lflag &= ~ECHO;    // Do not print after a key press.
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

  // Handle resize.
  on_resize = [&] { event_sender_->Send(Event::Special({0})); };
  install_signal_handler(SIGWINCH, OnResize);
#endif

  // Hide the cursor and show it at exit.
  std::cout << HIDE_CURSOR;
  std::cout << DISABLE_LINE_WRAP;
  std::cout << std::flush;
  on_exit_functions.push([&] {
    std::cout << reset_cursor_position;
    std::cout << SHOW_CURSOR;
    std::cout << ENABLE_LINE_WRAP;
    std::cout << std::endl;
  });

  // Produce a stream of Event from a stream of char.
  auto char_receiver = MakeReceiver<char>();
  auto char_sender = char_receiver->MakeSender();
  auto event_sender_1 = event_receiver_->MakeSender();
  auto char_to_event_stream = std::thread(
      CharToEventStream, std::move(char_receiver), std::move(event_sender_1));

  // Depending on the OS, start a thread that will produce events and/or chars.
#if defined(_WIN32)
  auto event_sender_2 = event_receiver_->MakeSender();
  auto event_listener =
      std::thread(&Win32EventListener, &quit_, std::move(char_sender),
                  std::move(event_sender_2));
#else
  auto event_listener =
      std::thread(&UnixEventListener, &quit_, std::move(char_sender));
#endif

  // The main loop.
  while (!quit_) {
    std::cout << reset_cursor_position << ResetPosition();
    Draw(component);
    std::cout << ToString() << set_cursor_position << std::flush;
    Clear();
    Event event;
    if (event_receiver_->Receive(&event))
      component->OnEvent(event);
  }

  char_to_event_stream.join();
  event_listener.join();
  OnExit(0);
}

void ScreenInteractive::Draw(Component* component) {
  auto document = component->Render();
  int dimx = 0;
  int dimy = 0;
  switch (dimension_) {
    case Dimension::Fixed:
      dimx = dimx_;
      dimy = dimy_;
      break;
    case Dimension::TerminalOutput:
      document->ComputeRequirement();
      dimx = Terminal::Size().dimx;
      dimy = document->requirement().min.y;
      break;
    case Dimension::Fullscreen:
      dimx = Terminal::Size().dimx;
      dimy = Terminal::Size().dimy;
      break;
    case Dimension::FitComponent:
      auto terminal = Terminal::Size();
      document->ComputeRequirement();
      dimx = std::min(document->requirement().min.x, terminal.dimx);
      dimy = std::min(document->requirement().min.y, terminal.dimy);
      break;
  }

  // Resize the screen if needed.
  if (dimx != dimx_ || dimy != dimy_) {
    dimx_ = dimx;
    dimy_ = dimy;
    pixels_ = std::vector<std::vector<Pixel>>(dimy, std::vector<Pixel>(dimx));
    cursor_.x = dimx_ - 1;
    cursor_.y = dimy_ - 1;
  }

  Render(*this, document.get());

  // Set cursor position for user using tools to insert CJK characters.
  set_cursor_position = "";
  reset_cursor_position = "";

  int dx = dimx_ - 1 - cursor_.x;
  int dy = dimy_ - 1 - cursor_.y;

  if (dx != 0) {
    set_cursor_position += "\x1B[" + std::to_string(dx) + "D";
    reset_cursor_position += "\x1B[" + std::to_string(dx) + "C";
  }
  if (dy != 0) {
    set_cursor_position += "\x1B[" + std::to_string(dy) + "A";
    reset_cursor_position += "\x1B[" + std::to_string(dy) + "B";
  }
}

std::function<void()> ScreenInteractive::ExitLoopClosure() {
  return [this]() {
    quit_ = true;
    event_sender_.reset();
  };
}

}  // namespace ftxui.
