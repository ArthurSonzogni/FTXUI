#include "ftxui/component/screen_interactive.hpp"

#include <stdio.h>

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <thread>

#include "ftxui/component/component.hpp"
#include "ftxui/component/terminal_input_parser.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"

#if defined(_WIN32)
#define DEFINE_CONSOLEV2_PROPERTIES
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
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

namespace {

constexpr int timeout_milliseconds = 20;
constexpr int timeout_microseconds = timeout_milliseconds * 1000;
#if defined(_WIN32)

void EventListener(std::atomic<bool>* quit,
                        Sender<Event> out) {
  auto console = GetStdHandle(STD_INPUT_HANDLE);
  auto parser = TerminalInputParser(out->Clone());
  while (!*quit) {
    // Throttle ReadConsoleInput by waiting 250ms, this wait function will
    // return if there is input in the console.
    auto wait_result = WaitForSingleObject(console, timeout_milliseconds);
    if (wait_result == WAIT_TIMEOUT) {
      parser.Timeout(timeout_milliseconds);
      continue;
    }

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
          parser.Add((char)key_event.uChar.UnicodeChar);
        } break;
        case WINDOW_BUFFER_SIZE_EVENT:
          out->Send(Event::Special({0}));
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

int CheckStdinReady(int usec_timeout) {
  timeval tv = {0, usec_timeout};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

// Read char from the terminal.
void EventListener(std::atomic<bool>* quit, Sender<Event> out) {
  const int buffer_size = 100;

  auto parser = TerminalInputParser(std::move(out));

  while (!*quit) {
    if (!CheckStdinReady(timeout_microseconds)) {
      parser.Timeout(timeout_milliseconds);
      continue;
    }

    char buff[buffer_size];
    int l = read(fileno(stdin), buff, buffer_size);
    for (int i = 0; i < l; ++i)
      parser.Add(buff[i]);
  }
}

#endif

static const char* HIDE_CURSOR = "\x1B[?25l";
static const char* SHOW_CURSOR = "\x1B[?25h";

static const char* DISABLE_LINE_WRAP = "\x1B[7l";
static const char* ENABLE_LINE_WRAP = "\x1B[7h";

static const char* USE_ALTERNATIVE_SCREEN = "\x1B[?1049h";
static const char* USE_NORMAL_SCREEN = "\x1B[?1049l";

using SignalHandler = void(int);
std::stack<std::function<void()>> on_exit_functions;
void OnExit(int signal) {
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }

  if (signal == SIGINT)
    std::exit(SIGINT);
}

auto install_signal_handler = [](int sig, SignalHandler handler) {
  auto old_signal_handler = std::signal(sig, handler);
  on_exit_functions.push([&]() { std::signal(sig, old_signal_handler); });
};

std::function<void()> on_resize = [] {};
void OnResize(int /* signal */) {
  on_resize();
}

}  // namespace

ScreenInteractive::ScreenInteractive(int dimx,
                                     int dimy,
                                     Dimension dimension,
                                     bool use_alternative_screen)
    : Screen(dimx, dimy),
      dimension_(dimension),
      use_alternative_screen_(use_alternative_screen) {
  event_receiver_ = MakeReceiver<Event>();
  event_sender_ = event_receiver_->MakeSender();
}

ScreenInteractive::~ScreenInteractive() {}

// static
ScreenInteractive ScreenInteractive::FixedSize(int dimx, int dimy) {
  return ScreenInteractive(dimx, dimy, Dimension::Fixed, false);
}

// static
ScreenInteractive ScreenInteractive::Fullscreen() {
  return ScreenInteractive(0, 0, Dimension::Fullscreen, true);
}

// static
ScreenInteractive ScreenInteractive::TerminalOutput() {
  return ScreenInteractive(0, 0, Dimension::TerminalOutput, false);
}

// static
ScreenInteractive ScreenInteractive::FitComponent() {
  return ScreenInteractive(0, 0, Dimension::FitComponent, false);
}

void ScreenInteractive::PostEvent(Event event) {
  if (!quit_)
    event_sender_->Send(event);
}

void ScreenInteractive::Loop(Component* component) {
  // Install a SIGINT handler and restore the old handler on exit.
  auto old_sigint_handler = std::signal(SIGINT, OnExit);
  on_exit_functions.push(
      [old_sigint_handler]() { std::signal(SIGINT, old_sigint_handler); });

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

  // https://docs.microsoft.com/en-us/windows/console/setconsolemode
  const int enable_virtual_terminal_processing = 0x0004;
  const int disable_newline_auto_return = 0x0008;
  out_mode |= enable_virtual_terminal_processing;
  out_mode |= disable_newline_auto_return;

  // https://docs.microsoft.com/en-us/windows/console/setconsolemode
  const int enable_line_input = 0x0002;
  const int enable_echo_input = 0x0004;
  const int enable_virtual_terminal_input = 0x0200;
  const int enable_window_input = 0x0008;
  in_mode &= ~enable_echo_input;
  in_mode &= ~enable_line_input;
  in_mode |= enable_virtual_terminal_input;
  in_mode |= enable_window_input;

  SetConsoleMode(stdin_handle, in_mode);
  SetConsoleMode(stdout_handle, out_mode);
#else
  struct termios terminal;
  tcgetattr(STDIN_FILENO, &terminal);
  on_exit_functions.push([=] { tcsetattr(STDIN_FILENO, TCSANOW, &terminal); });

  terminal.c_lflag &= ~ICANON;  // Non canonique terminal.
  terminal.c_lflag &= ~ECHO;    // Do not print after a key press.
  terminal.c_cc[VMIN] = 0;
  terminal.c_cc[VTIME] = 0;
  // auto oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  // fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  // on_exit_functions.push([=] { fcntl(STDIN_FILENO, F_GETFL, oldf); });

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

  auto event_listener =
      std::thread(&EventListener, &quit_, event_receiver_->MakeSender());

  if (use_alternative_screen_) {
    std::cout << USE_ALTERNATIVE_SCREEN;
    on_exit_functions.push([] { std::cout << USE_NORMAL_SCREEN; });
  }

  // The main loop.
  while (!quit_) {
    if (!event_receiver_->HasPending()) {
      std::cout << reset_cursor_position << ResetPosition();
      Draw(component);
      std::cout << ToString() << set_cursor_position << std::flush;
      Clear();
    }
    Event event;
    if (event_receiver_->Receive(&event))
      component->OnEvent(event);
  }

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
      dimy = document->requirement().min_y;
      break;
    case Dimension::Fullscreen:
      dimx = Terminal::Size().dimx;
      dimy = Terminal::Size().dimy;
      break;
    case Dimension::FitComponent:
      auto terminal = Terminal::Size();
      document->ComputeRequirement();
      dimx = std::min(document->requirement().min_x, terminal.dimx);
      dimy = std::min(document->requirement().min_y, terminal.dimy);
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

  Render(*this, document);

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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
