#include <algorithm>  // for copy, max, min
#include <array>      // for array
#include <chrono>  // for operator-, milliseconds, duration, operator>=, time_point, common_type<>::type
#include <csignal>  // for signal, raise, SIGTSTP, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM, SIGWINCH
#include <cstdio>   // for fileno, size_t, stdin
#include <functional>        // for function
#include <initializer_list>  // for initializer_list
#include <iostream>  // for cout, ostream, basic_ostream, operator<<, endl, flush
#include <stack>     // for stack
#include <thread>    // for thread, sleep_for
#include <type_traits>  // for decay_t
#include <utility>      // for move, swap
#include <variant>      // for visit
#include <vector>       // for vector

#include "ftxui/component/animation.hpp"  // for TimePoint, Clock, Duration, Params, RequestAnimationFrame
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse, CapturedMouseInterface
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/receiver.hpp"  // for ReceiverImpl, Sender, MakeReceiver, SenderImpl, Receiver
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/terminal_input_parser.hpp"  // for TerminalInputParser
#include "ftxui/dom/node.hpp"                         // for Node, Render
#include "ftxui/dom/requirement.hpp"                  // for Requirement
#include "ftxui/screen/terminal.hpp"                  // for Size, Dimensions

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
#include <sys/select.h>  // for select, FD_ISSET, FD_SET, FD_ZERO, fd_set
#include <termios.h>  // for tcsetattr, termios, tcgetattr, TCSANOW, cc_t, ECHO, ICANON, VMIN, VTIME
#include <unistd.h>  // for STDIN_FILENO, read
#endif

// Quick exit is missing in standard CLang headers
#if defined(__clang__) && defined(__APPLE__)
#define quick_exit(a) exit(a)
#endif

namespace ftxui {

namespace animation {
void RequestAnimationFrame() {
  auto* screen = ScreenInteractive::Active();
  if (screen) {
    screen->RequestAnimationFrame();
  }
}
}  // namespace animation

namespace {

ScreenInteractive* g_active_screen = nullptr;  // NOLINT

void Flush() {
  // Emscripten doesn't implement flush. We interpret zero as flush.
  std::cout << '\0' << std::flush;
}

constexpr int timeout_milliseconds = 20;
constexpr int timeout_microseconds = timeout_milliseconds * 1000;
#if defined(_WIN32)

void EventListener(std::atomic<bool>* quit, Sender<Task> out) {
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
    ReadConsoleInput(console, records.data(), (DWORD)records.size(),
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

#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>

// Read char from the terminal.
void EventListener(std::atomic<bool>* quit, Sender<Task> out) {
  (void)timeout_microseconds;
  auto parser = TerminalInputParser(std::move(out));

  char c;
  while (!*quit) {
    while (read(STDIN_FILENO, &c, 1), c)
      parser.Add(c);

    emscripten_sleep(1);
    parser.Timeout(1);
  }
}

#else
#include <sys/time.h>  // for timeval

int CheckStdinReady(int usec_timeout) {
  timeval tv = {0, usec_timeout};
  fd_set fds;
  FD_ZERO(&fds);                                          // NOLINT
  FD_SET(STDIN_FILENO, &fds);                             // NOLINT
  select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);  // NOLINT
  return FD_ISSET(STDIN_FILENO, &fds);                    // NOLINT
}

// Read char from the terminal.
void EventListener(std::atomic<bool>* quit, Sender<Task> out) {
  auto parser = TerminalInputParser(std::move(out));

  while (!*quit) {
    if (!CheckStdinReady(timeout_microseconds)) {
      parser.Timeout(timeout_milliseconds);
      continue;
    }

    const size_t buffer_size = 100;
    std::array<char, buffer_size> buffer;                     // NOLINT;
    int l = read(fileno(stdin), buffer.data(), buffer_size);  // NOLINT
    for (int i = 0; i < l; ++i) {
      parser.Add(buffer[i]);  // NOLINT
    }
  }
}

#endif

const std::string CSI = "\x1b[";  // NOLINT

// DEC: Digital Equipment Corporation
enum class DECMode {
  kLineWrap = 7,
  kMouseX10 = 9,
  kCursor = 25,
  kMouseVt200 = 1000,
  kMouseAnyEvent = 1003,
  kMouseUtf8 = 1005,
  kMouseSgrExtMode = 1006,
  kMouseUrxvtMode = 1015,
  kMouseSgrPixelsMode = 1016,
  kAlternateScreen = 1049,
};

// Device Status Report (DSR) {
enum class DSRMode {
  kCursor = 6,
};

std::string Serialize(const std::vector<DECMode>& parameters) {
  bool first = true;
  std::string out;
  for (DECMode parameter : parameters) {
    if (!first) {
      out += ";";
    }
    out += std::to_string(int(parameter));
    first = false;
  }
  return out;
}

// DEC Private Mode Set (DECSET)
std::string Set(const std::vector<DECMode>& parameters) {
  return CSI + "?" + Serialize(parameters) + "h";
}

// DEC Private Mode Reset (DECRST)
std::string Reset(const std::vector<DECMode>& parameters) {
  return CSI + "?" + Serialize(parameters) + "l";
}

// Device Status Report (DSR)
std::string DeviceStatusReport(DSRMode ps) {
  return CSI + std::to_string(int(ps)) + "n";
}

using SignalHandler = void(int);
std::stack<Closure> on_exit_functions;  // NOLINT
void OnExit(int signal) {
  (void)signal;
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }
}

const auto install_signal_handler = [](int sig, SignalHandler handler) {
  auto old_signal_handler = std::signal(sig, handler);
  on_exit_functions.push([=] { std::signal(sig, old_signal_handler); });
};

Closure g_on_resize = [] {};  // NOLINT
void OnResize(int /* signal */) {
  g_on_resize();
}

void OnSigStop(int /*signal*/) {
  ScreenInteractive::Private::SigStop(*g_active_screen);
}

class CapturedMouseImpl : public CapturedMouseInterface {
 public:
  explicit CapturedMouseImpl(std::function<void(void)> callback)
      : callback_(std::move(callback)) {}
  ~CapturedMouseImpl() override { callback_(); }
  CapturedMouseImpl(const CapturedMouseImpl&) = delete;
  CapturedMouseImpl(CapturedMouseImpl&&) = delete;
  CapturedMouseImpl& operator=(const CapturedMouseImpl&) = delete;
  CapturedMouseImpl& operator=(CapturedMouseImpl&&) = delete;

 private:
  std::function<void(void)> callback_;
};

void AnimationListener(std::atomic<bool>* quit, Sender<Task> out) {
  // Animation at around 60fps.
  const auto time_delta = std::chrono::milliseconds(15);
  while (!*quit) {
    out->Send(AnimationTask());
    std::this_thread::sleep_for(time_delta);
  }
}

}  // namespace

ScreenInteractive::ScreenInteractive(int dimx,
                                     int dimy,
                                     Dimension dimension,
                                     bool use_alternative_screen)
    : Screen(dimx, dimy),
      dimension_(dimension),
      use_alternative_screen_(use_alternative_screen) {
  task_receiver_ = MakeReceiver<Task>();
}

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

void ScreenInteractive::Post(Task task) {
  // Task/Events sent toward inactive screen or screen waiting to become
  // inactive are dropped.
  if (!task_sender_) {
    return;
  }

  task_sender_->Send(std::move(task));
}

void ScreenInteractive::PostEvent(Event event) {
  Post(event);
}

void ScreenInteractive::RequestAnimationFrame() {
  if (animation_requested_) {
    return;
  }
  animation_requested_ = true;
  auto now = animation::Clock::now();
  const auto time_histeresis = std::chrono::milliseconds(33);
  if (now - previous_animation_time >= time_histeresis) {
    previous_animation_time = now;
  }
}

CapturedMouse ScreenInteractive::CaptureMouse() {
  if (mouse_captured) {
    return nullptr;
  }
  mouse_captured = true;
  return std::make_unique<CapturedMouseImpl>(
      [this] { mouse_captured = false; });
}

void ScreenInteractive::Loop(Component component) {  // NOLINT
  // Suspend previously active screen:
  if (g_active_screen) {
    std::swap(suspended_screen_, g_active_screen);
    std::cout << suspended_screen_->reset_cursor_position
              << suspended_screen_->ResetPosition(/*clear=*/true);
    suspended_screen_->dimx_ = 0;
    suspended_screen_->dimy_ = 0;
    suspended_screen_->Uninstall();
  }

  // This screen is now active:
  g_active_screen = this;
  g_active_screen->Install();
  g_active_screen->Main(std::move(component));
  g_active_screen->Uninstall();
  g_active_screen = nullptr;

  // Put cursor position at the end of the drawing.
  std::cout << reset_cursor_position;

  // Restore suspended screen.
  if (suspended_screen_) {
    std::cout << ResetPosition(/*clear=*/true);
    dimx_ = 0;
    dimy_ = 0;
    std::swap(g_active_screen, suspended_screen_);
    g_active_screen->Install();
  } else {
    // On final exit, keep the current drawing and reset cursor position one
    // line after it.
    std::cout << std::endl;
  }
}

/// @brief Decorate a function. It executes the same way, but with the currently
/// active screen terminal hooks temporarilly uninstalled during its execution.
/// @param fn The function to decorate.
Closure ScreenInteractive::WithRestoredIO(Closure fn) {  // NOLINT
  return [this, fn] {
    Uninstall();
    fn();
    Install();
  };
}

// static
ScreenInteractive* ScreenInteractive::Active() {
  return g_active_screen;
}

void ScreenInteractive::Install() {
  // After uninstalling the new configuration, flush it to the terminal to
  // ensure it is fully applied:
  on_exit_functions.push([] { Flush(); });

  on_exit_functions.push([this] { ExitLoopClosure()(); });

  // Install signal handlers to restore the terminal state on exit. The default
  // signal handlers are restored on exit.
  for (int signal : {SIGTERM, SIGSEGV, SIGINT, SIGILL, SIGABRT, SIGFPE}) {
    install_signal_handler(signal, OnExit);
  }

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
  struct termios terminal;  // NOLINT
  tcgetattr(STDIN_FILENO, &terminal);
  on_exit_functions.push([=] { tcsetattr(STDIN_FILENO, TCSANOW, &terminal); });

  terminal.c_lflag &= ~ICANON;  // NOLINT Non canonique terminal.
  terminal.c_lflag &= ~ECHO;    // NOLINT Do not print after a key press.
  terminal.c_cc[VMIN] = 0;
  terminal.c_cc[VTIME] = 0;
  // auto oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  // fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  // on_exit_functions.push([=] { fcntl(STDIN_FILENO, F_GETFL, oldf); });

  tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

  // Handle resize.
  g_on_resize = [&] { task_sender_->Send(Event::Special({0})); };
  install_signal_handler(SIGWINCH, OnResize);

  // Handle SIGTSTP/SIGCONT.
  install_signal_handler(SIGTSTP, OnSigStop);
#endif

  auto enable = [&](const std::vector<DECMode>& parameters) {
    std::cout << Set(parameters);
    on_exit_functions.push([=] { std::cout << Reset(parameters); });
  };

  auto disable = [&](const std::vector<DECMode>& parameters) {
    std::cout << Reset(parameters);
    on_exit_functions.push([=] { std::cout << Set(parameters); });
  };

  if (use_alternative_screen_) {
    enable({
        DECMode::kAlternateScreen,
    });
  }

  disable({
      DECMode::kCursor,
      DECMode::kLineWrap,
  });

  enable({
      // DECMode::kMouseVt200,
      DECMode::kMouseAnyEvent,
      DECMode::kMouseUtf8,
      DECMode::kMouseSgrExtMode,
  });

  // After installing the new configuration, flush it to the terminal to ensure
  // it is fully applied:
  Flush();

  quit_ = false;
  task_sender_ = task_receiver_->MakeSender();
  event_listener_ =
      std::thread(&EventListener, &quit_, task_receiver_->MakeSender());
  animation_listener_ =
      std::thread(&AnimationListener, &quit_, task_receiver_->MakeSender());
}

void ScreenInteractive::Uninstall() {
  ExitLoopClosure()();
  event_listener_.join();
  animation_listener_.join();

  OnExit(0);
}

// NOLINTNEXTLINE
void ScreenInteractive::Main(Component component) {
  previous_animation_time = animation::Clock::now();

  auto draw = [&] {
    Draw(component);
    std::cout << ToString() << set_cursor_position;
    Flush();
    Clear();
  };

  bool attempt_draw = true;
  while (!quit_) {
    if (attempt_draw && !task_receiver_->HasPending()) {
      draw();
      attempt_draw = false;
    }

    Task task;
    if (!task_receiver_->Receive(&task)) {
      break;
    }

    // clang-format off
    std::visit([&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;

      // Handle Event.
      if constexpr (std::is_same_v<T, Event>) {
        if (arg.is_cursor_reporting()) {
          cursor_x_ = arg.cursor_x();
          cursor_y_ = arg.cursor_y();
          return;
        }

        if (arg.is_mouse()) {
          arg.mouse().x -= cursor_x_;
          arg.mouse().y -= cursor_y_;
        }

        arg.screen_ = this;
        component->OnEvent(arg);
        attempt_draw = true;
        return;
      }

      // Handle callback
      if constexpr (std::is_same_v<T, Closure>) {
        arg();
        return;
      }

      // Handle Animation
      if constexpr (std::is_same_v<T, AnimationTask>) {
        if (!animation_requested_) {
          return;
        }

        animation_requested_ = false;
        animation::TimePoint now = animation::Clock::now();
        animation::Duration delta = now - previous_animation_time;
        previous_animation_time = now;

        animation::Params params(delta);
        component->OnAnimation(params);
        attempt_draw = true;
        return;
      }
    },
    task);
    // clang-format on
  }
}

// NOLINTNEXTLINE
void ScreenInteractive::Draw(Component component) {
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

  bool resized = (dimx != dimx_) || (dimy != dimy_);
  std::cout << reset_cursor_position << ResetPosition(/*clear=*/resized);

  // Resize the screen if needed.
  if (resized) {
    dimx_ = dimx;
    dimy_ = dimy;
    pixels_ = std::vector<std::vector<Pixel>>(dimy, std::vector<Pixel>(dimx));
    cursor_.x = dimx_ - 1;
    cursor_.y = dimy_ - 1;
  }

  // Periodically request the terminal emulator the frame position relative to
  // the screen. This is useful for converting mouse position reported in
  // screen's coordinates to frame's coordinates.
#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  // Microsoft's terminal suffers from a [bug]. When reporting the cursor
  // position, several output sequences are mixed together into garbage.
  // This causes FTXUI user to see some "1;1;R" sequences into the Input
  // component. See [issue]. Solution is to request cursor position less
  // often. [bug]: https://github.com/microsoft/terminal/pull/7583 [issue]:
  // https://github.com/ArthurSonzogni/FTXUI/issues/136
  static int i = -3;
  ++i;
  if (!use_alternative_screen_ && (i % 150 == 0)) {  // NOLINT
    std::cout << DeviceStatusReport(DSRMode::kCursor);
  }
#else
  static int i = -3;
  ++i;
  if (!use_alternative_screen_ &&
      (previous_frame_resized_ || i % 40 == 0)) {  // NOLINT
    std::cout << DeviceStatusReport(DSRMode::kCursor);
  }
#endif
  previous_frame_resized_ = resized;

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

Closure ScreenInteractive::ExitLoopClosure() {
  return [this] {
    quit_ = true;
    task_sender_.reset();
  };
}

void ScreenInteractive::SigStop() {
#if defined(_WIN32)
  // Windows do no support SIGTSTP.
#else
  Post([&] {
    Uninstall();
    std::cout << reset_cursor_position;
    reset_cursor_position = "";
    std::cout << ResetPosition(/*clear=*/true);
    dimx_ = 0;
    dimy_ = 0;
    Flush();
    std::raise(SIGTSTP);
    Install();
  });
#endif
}

}  // namespace ftxui.

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
