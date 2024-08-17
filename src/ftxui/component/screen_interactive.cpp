// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/screen_interactive.hpp"
#include <algorithm>  // for copy, max, min
#include <array>      // for array
#include <atomic>
#include <chrono>  // for operator-, milliseconds, operator>=, duration, common_type<>::type, time_point
#include <csignal>  // for signal, SIGTSTP, SIGABRT, SIGWINCH, raise, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM, __sighandler_t, size_t
#include <cstdint>
#include <cstdio>                    // for fileno, stdin
#include <ftxui/component/task.hpp>  // for Task, Closure, AnimationTask
#include <ftxui/screen/screen.hpp>  // for Pixel, Screen::Cursor, Screen, Screen::Cursor::Hidden
#include <functional>        // for function
#include <initializer_list>  // for initializer_list
#include <iostream>  // for cout, ostream, operator<<, basic_ostream, endl, flush
#include <memory>
#include <stack>  // for stack
#include <string>
#include <thread>       // for thread, sleep_for
#include <tuple>        // for _Swallow_assign, ignore
#include <type_traits>  // for decay_t
#include <utility>      // for move, swap
#include <variant>      // for visit, variant
#include <vector>       // for vector
#include "ftxui/component/animation.hpp"  // for TimePoint, Clock, Duration, Params, RequestAnimationFrame
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse, CapturedMouseInterface
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/loop.hpp"            // for Loop
#include "ftxui/component/receiver.hpp"  // for ReceiverImpl, Sender, MakeReceiver, SenderImpl, Receiver
#include "ftxui/component/terminal_input_parser.hpp"  // for TerminalInputParser
#include "ftxui/dom/node.hpp"                         // for Node, Render
#include "ftxui/dom/requirement.hpp"                  // for Requirement
#include "ftxui/screen/pixel.hpp"                     // for Pixel
#include "ftxui/screen/terminal.hpp"                  // for Dimensions, Size

#if defined(_WIN32)
#define DEFINE_CONSOLEV2_PROPERTIES
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifndef UNICODE
#error Must be compiled in UNICODE mode
#endif
#else
#include <sys/select.h>  // for select, FD_ISSET, FD_SET, FD_ZERO, fd_set, timeval
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
[[maybe_unused]] constexpr int timeout_microseconds =
    timeout_milliseconds * 1000;
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
          std::wstring wstring;
          wstring += key_event.uChar.UnicodeChar;
          for (auto it : to_string(wstring)) {
            parser.Add(it);
          }
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
  auto parser = TerminalInputParser(std::move(out));

  char c;
  while (!*quit) {
    while (read(STDIN_FILENO, &c, 1), c)
      parser.Add(c);

    emscripten_sleep(1);
    parser.Timeout(1);
  }
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void ftxui_on_resize(int columns, int rows) {
  Terminal::SetFallbackSize({
      columns,
      rows,
  });
  std::raise(SIGWINCH);
}
}

#else  // POSIX (Linux & Mac)

int CheckStdinReady(int usec_timeout) {
  timeval tv = {0, usec_timeout};  // NOLINT
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
    std::array<char, buffer_size> buffer;                        // NOLINT;
    size_t l = read(fileno(stdin), buffer.data(), buffer_size);  // NOLINT
    for (size_t i = 0; i < l; ++i) {
      parser.Add(buffer[i]);  // NOLINT
    }
  }
}
#endif

std::stack<Closure> on_exit_functions;  // NOLINT
void OnExit() {
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }
}

std::atomic<int> g_signal_exit_count = 0;  // NOLINT
#if !defined(_WIN32)
std::atomic<int> g_signal_stop_count = 0;    // NOLINT
std::atomic<int> g_signal_resize_count = 0;  // NOLINT
#endif

// Async signal safe function
void RecordSignal(int signal) {
  switch (signal) {
    case SIGABRT:
    case SIGFPE:
    case SIGILL:
    case SIGINT:
    case SIGSEGV:
    case SIGTERM:
      g_signal_exit_count++;
      break;

#if !defined(_WIN32)
    case SIGTSTP:  // NOLINT
      g_signal_stop_count++;
      break;

    case SIGWINCH:  // NOLINT
      g_signal_resize_count++;
      break;
#endif

    default:
      break;
  }
}

void ExecuteSignalHandlers() {
  int signal_exit_count = g_signal_exit_count.exchange(0);
  while (signal_exit_count--) {
    ScreenInteractive::Private::Signal(*g_active_screen, SIGABRT);
  }

#if !defined(_WIN32)
  int signal_stop_count = g_signal_stop_count.exchange(0);
  while (signal_stop_count--) {
    ScreenInteractive::Private::Signal(*g_active_screen, SIGTSTP);
  }

  int signal_resize_count = g_signal_resize_count.exchange(0);
  while (signal_resize_count--) {
    ScreenInteractive::Private::Signal(*g_active_screen, SIGWINCH);
  }
#endif
}

void InstallSignalHandler(int sig) {
  auto old_signal_handler = std::signal(sig, RecordSignal);
  on_exit_functions.emplace(
      [=] { std::ignore = std::signal(sig, old_signal_handler); });
}

// CSI: Control Sequence Introducer
const std::string CSI = "\x1b[";  // NOLINT
                                  //
// DCS: Device Control String
const std::string DCS = "\x1bP";  // NOLINT
// ST: String Terminator
const std::string ST = "\x1b\\";  // NOLINT

// DECRQSS: Request Status String
// DECSCUSR: Set Cursor Style
const std::string DECRQSS_DECSCUSR = DCS + "$q q" + ST;  // NOLINT

// DEC: Digital Equipment Corporation
enum class DECMode : std::uint16_t {
  kLineWrap = 7,
  kCursor = 25,

  kMouseX10 = 9,
  kMouseVt200 = 1000,
  kMouseVt200Highlight = 1001,

  kMouseBtnEventMouse = 1002,
  kMouseAnyEvent = 1003,

  kMouseUtf8 = 1005,
  kMouseSgrExtMode = 1006,
  kMouseUrxvtMode = 1015,
  kMouseSgrPixelsMode = 1016,
  kAlternateScreen = 1049,
};

// Device Status Report (DSR) {
enum class DSRMode : std::uint8_t {
  kCursor = 6,
};

std::string Serialize(const std::vector<DECMode>& parameters) {
  bool first = true;
  std::string out;
  for (const DECMode parameter : parameters) {
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
  return {
      dimx,
      dimy,
      Dimension::Fixed,
      false,
  };
}

/// @ingroup component
/// Create a ScreenInteractive taking the full terminal size. This is using the
/// alternate screen buffer to avoid messing with the terminal content.
/// @note This is the same as `ScreenInteractive::FullscreenAlternateScreen()`
// static
ScreenInteractive ScreenInteractive::Fullscreen() {
  return FullscreenAlternateScreen();
}

/// @ingroup component
/// Create a ScreenInteractive taking the full terminal size. The primary screen
/// buffer is being used. It means if the terminal is resized, the previous
/// content might mess up with the terminal content.
// static
ScreenInteractive ScreenInteractive::FullscreenPrimaryScreen() {
  return {
      0,
      0,
      Dimension::Fullscreen,
      false,
  };
}

/// @ingroup component
/// Create a ScreenInteractive taking the full terminal size. This is using the
/// alternate screen buffer to avoid messing with the terminal content.
// static
ScreenInteractive ScreenInteractive::FullscreenAlternateScreen() {
  return {
      0,
      0,
      Dimension::Fullscreen,
      true,
  };
}

// static
ScreenInteractive ScreenInteractive::TerminalOutput() {
  return {
      0,
      0,
      Dimension::TerminalOutput,
      false,
  };
}

// static
ScreenInteractive ScreenInteractive::FitComponent() {
  return {
      0,
      0,
      Dimension::FitComponent,
      false,
  };
}

/// @ingroup component
/// @brief Set whether mouse is tracked and events reported.
/// called outside of the main loop. E.g `ScreenInteractive::Loop(...)`.
/// @param enable Whether to enable mouse event tracking.
/// @note This muse be called outside of the main loop. E.g. before calling
/// `ScreenInteractive::Loop`.
/// @note Mouse tracking is enabled by default.
/// @note Mouse tracking is only supported on terminals that supports it.
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// screen.TrackMouse(false);
/// screen.Loop(component);
/// ```
void ScreenInteractive::TrackMouse(bool enable) {
  track_mouse_ = enable;
}

/// @brief Add a task to the main loop.
/// It will be executed later, after every other scheduled tasks.
/// @ingroup component
void ScreenInteractive::Post(Task task) {
  // Task/Events sent toward inactive screen or screen waiting to become
  // inactive are dropped.
  if (!task_sender_) {
    return;
  }

  task_sender_->Send(std::move(task));
}

/// @brief Add an event to the main loop.
/// It will be executed later, after every other scheduled events.
/// @ingroup component
void ScreenInteractive::PostEvent(Event event) {
  Post(event);
}

/// @brief Add a task to draw the screen one more time, until all the animations
/// are done.
void ScreenInteractive::RequestAnimationFrame() {
  if (animation_requested_) {
    return;
  }
  animation_requested_ = true;
  auto now = animation::Clock::now();
  const auto time_histeresis = std::chrono::milliseconds(33);
  if (now - previous_animation_time_ >= time_histeresis) {
    previous_animation_time_ = now;
  }
}

/// @brief Try to get the unique lock about behing able to capture the mouse.
/// @return A unique lock if the mouse is not already captured, otherwise a
/// null.
/// @ingroup component
CapturedMouse ScreenInteractive::CaptureMouse() {
  if (mouse_captured) {
    return nullptr;
  }
  mouse_captured = true;
  return std::make_unique<CapturedMouseImpl>(
      [this] { mouse_captured = false; });
}

/// @brief Execute the main loop.
/// @param component The component to draw.
/// @ingroup component
void ScreenInteractive::Loop(Component component) {  // NOLINT
  class Loop loop(this, std::move(component));
  loop.Run();
}

/// @brief Return whether the main loop has been quit.
/// @ingroup component
bool ScreenInteractive::HasQuitted() {
  return task_receiver_->HasQuitted();
}

// private
void ScreenInteractive::PreMain() {
  // Suspend previously active screen:
  if (g_active_screen) {
    std::swap(suspended_screen_, g_active_screen);
    // Reset cursor position to the top of the screen and clear the screen.
    suspended_screen_->ResetCursorPosition();
    std::cout << suspended_screen_->ResetPosition(/*clear=*/true);
    suspended_screen_->dimx_ = 0;
    suspended_screen_->dimy_ = 0;

    // Reset dimensions to force drawing the screen again next time:
    suspended_screen_->Uninstall();
  }

  // This screen is now active:
  g_active_screen = this;
  g_active_screen->Install();

  previous_animation_time_ = animation::Clock::now();
}

// private
void ScreenInteractive::PostMain() {
  // Put cursor position at the end of the drawing.
  ResetCursorPosition();

  g_active_screen = nullptr;

  // Restore suspended screen.
  if (suspended_screen_) {
    // Clear screen, and put the cursor at the beginning of the drawing.
    std::cout << ResetPosition(/*clear=*/true);
    dimx_ = 0;
    dimy_ = 0;
    Uninstall();
    std::swap(g_active_screen, suspended_screen_);
    g_active_screen->Install();
  } else {
    Uninstall();

    std::cout << '\r';
    // On final exit, keep the current drawing and reset cursor position one
    // line after it.
    if (!use_alternative_screen_) {
      std::cout << '\n';
      std::cout << std::flush;
    }
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

/// @brief Force FTXUI to handle or not handle Ctrl-C, even if the component
/// catches the Event::CtrlC.
void ScreenInteractive::ForceHandleCtrlC(bool force) {
  force_handle_ctrl_c_ = force;
}

/// @brief Force FTXUI to handle or not handle Ctrl-Z, even if the component
/// catches the Event::CtrlZ.
void ScreenInteractive::ForceHandleCtrlZ(bool force) {
  force_handle_ctrl_z_ = force;
}

/// @brief Return the currently active screen, or null if none.
// static
ScreenInteractive* ScreenInteractive::Active() {
  return g_active_screen;
}

// private
void ScreenInteractive::Install() {
  frame_valid_ = false;

  // Flush the buffer for stdout to ensure whatever the user has printed before
  // is fully applied before we start modifying the terminal configuration. This
  // is important, because we are using two different channels (stdout vs
  // termios/WinAPI) to communicate with the terminal emulator below. See
  // https://github.com/ArthurSonzogni/FTXUI/issues/846
  Flush();

  // After uninstalling the new configuration, flush it to the terminal to
  // ensure it is fully applied:
  on_exit_functions.emplace([] { Flush(); });

  on_exit_functions.emplace([this] { ExitLoopClosure()(); });

  // Request the terminal to report the current cursor shape. We will restore it
  // on exit.
  std::cout << DECRQSS_DECSCUSR;
  on_exit_functions.emplace([this] {
    std::cout << "\033[?25h";  // Enable cursor.
    std::cout << "\033[" + std::to_string(cursor_reset_shape_) + " q";
  });

  // Install signal handlers to restore the terminal state on exit. The default
  // signal handlers are restored on exit.
  for (const int signal : {SIGTERM, SIGSEGV, SIGINT, SIGILL, SIGABRT, SIGFPE}) {
    InstallSignalHandler(signal);
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
  for (const int signal : {SIGWINCH, SIGTSTP}) {
    InstallSignalHandler(signal);
  }

  struct termios terminal;  // NOLINT
  tcgetattr(STDIN_FILENO, &terminal);
  on_exit_functions.emplace(
      [=] { tcsetattr(STDIN_FILENO, TCSANOW, &terminal); });

  // Enabling raw terminal input mode
  terminal.c_iflag &= ~IGNBRK;  // Disable ignoring break condition
  terminal.c_iflag &= ~BRKINT;  // Disable break causing input and output to be
                                // flushed
  terminal.c_iflag &= ~PARMRK;  // Disable marking parity errors.
  terminal.c_iflag &= ~ISTRIP;  // Disable striping 8th bit off characters.
  terminal.c_iflag &= ~INLCR;   // Disable mapping NL to CR.
  terminal.c_iflag &= ~IGNCR;   // Disable ignoring CR.
  terminal.c_iflag &= ~ICRNL;   // Disable mapping CR to NL.
  terminal.c_iflag &= ~IXON;    // Disable XON/XOFF flow control on output

  terminal.c_lflag &= ~ECHO;    // Disable echoing input characters.
  terminal.c_lflag &= ~ECHONL;  // Disable echoing new line characters.
  terminal.c_lflag &= ~ICANON;  // Disable Canonical mode.
  terminal.c_lflag &= ~ISIG;    // Disable sending signal when hitting:
                                // -     => DSUSP
                                // - C-Z => SUSP
                                // - C-C => INTR
                                // - C-d => QUIT
  terminal.c_lflag &= ~IEXTEN;  // Disable extended input processing
  terminal.c_cflag |= CS8;      // 8 bits per byte

  terminal.c_cc[VMIN] = 0;   // Minimum number of characters for non-canonical
                             // read.
  terminal.c_cc[VTIME] = 0;  // Timeout in deciseconds for non-canonical read.

  tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

#endif

  auto enable = [&](const std::vector<DECMode>& parameters) {
    std::cout << Set(parameters);
    on_exit_functions.emplace([=] { std::cout << Reset(parameters); });
  };

  auto disable = [&](const std::vector<DECMode>& parameters) {
    std::cout << Reset(parameters);
    on_exit_functions.emplace([=] { std::cout << Set(parameters); });
  };

  if (use_alternative_screen_) {
    enable({
        DECMode::kAlternateScreen,
    });
  }

  disable({
      // DECMode::kCursor,
      DECMode::kLineWrap,
  });

  if (track_mouse_) {
    enable({DECMode::kMouseVt200});
    enable({DECMode::kMouseAnyEvent});
    enable({DECMode::kMouseUrxvtMode});
    enable({DECMode::kMouseSgrExtMode});
  }

  // After installing the new configuration, flush it to the terminal to
  // ensure it is fully applied:
  Flush();

  quit_ = false;
  task_sender_ = task_receiver_->MakeSender();
  event_listener_ =
      std::thread(&EventListener, &quit_, task_receiver_->MakeSender());
  animation_listener_ =
      std::thread(&AnimationListener, &quit_, task_receiver_->MakeSender());
}

// private
void ScreenInteractive::Uninstall() {
  ExitNow();
  event_listener_.join();
  animation_listener_.join();
  OnExit();
}

// private
// NOLINTNEXTLINE
void ScreenInteractive::RunOnceBlocking(Component component) {
  ExecuteSignalHandlers();
  Task task;
  if (task_receiver_->Receive(&task)) {
    HandleTask(component, task);
  }
  RunOnce(component);
}

// private
void ScreenInteractive::RunOnce(Component component) {
  Task task;
  while (task_receiver_->ReceiveNonBlocking(&task)) {
    HandleTask(component, task);
    ExecuteSignalHandlers();
  }
  Draw(std::move(component));
}

// private
// NOLINTNEXTLINE
void ScreenInteractive::HandleTask(Component component, Task& task) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        // clang-format off
    // Handle Event.
    if constexpr (std::is_same_v<T, Event>) {
      if (arg.is_cursor_position()) {
        cursor_x_ = arg.cursor_x();
        cursor_y_ = arg.cursor_y();
        return;
      }

      if (arg.is_cursor_shape()) {
        cursor_reset_shape_= arg.cursor_shape();
        return;
      }

      if (arg.is_mouse()) {
        arg.mouse().x -= cursor_x_;
        arg.mouse().y -= cursor_y_;
      }

      arg.screen_ = this;

      const bool handled = component->OnEvent(arg);

      if (arg == Event::CtrlC && (!handled || force_handle_ctrl_c_)) {
        RecordSignal(SIGABRT);
      }

#if !defined(_WIN32)
      if (arg == Event::CtrlZ && (!handled || force_handle_ctrl_z_)) {
        RecordSignal(SIGTSTP);
      }
#endif
      
      frame_valid_ = false;
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
      const animation::TimePoint now = animation::Clock::now();
      const animation::Duration delta = now - previous_animation_time_;
      previous_animation_time_ = now;

      animation::Params params(delta);
      component->OnAnimation(params);
      frame_valid_ = false;
      return;
    }
  },
  task);
  // clang-format on
}

// private
// NOLINTNEXTLINE
void ScreenInteractive::Draw(Component component) {
  if (frame_valid_) {
    return;
  }
  auto document = component->Render();
  int dimx = 0;
  int dimy = 0;
  auto terminal = Terminal::Size();
  document->ComputeRequirement();
  switch (dimension_) {
    case Dimension::Fixed:
      dimx = dimx_;
      dimy = dimy_;
      break;
    case Dimension::TerminalOutput:
      dimx = terminal.dimx;
      dimy = document->requirement().min_y;
      break;
    case Dimension::Fullscreen:
      dimx = terminal.dimx;
      dimy = terminal.dimy;
      break;
    case Dimension::FitComponent:
      dimx = std::min(document->requirement().min_x, terminal.dimx);
      dimy = std::min(document->requirement().min_y, terminal.dimy);
      break;
  }

  const bool resized = (dimx != dimx_) || (dimy != dimy_);
  ResetCursorPosition();
  std::cout << ResetPosition(/*clear=*/resized);

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
  {
    const int dx = dimx_ - 1 - cursor_.x + int(dimx_ != terminal.dimx);
    const int dy = dimy_ - 1 - cursor_.y;

    set_cursor_position.clear();
    reset_cursor_position.clear();

    if (dy != 0) {
      set_cursor_position += "\x1B[" + std::to_string(dy) + "A";
      reset_cursor_position += "\x1B[" + std::to_string(dy) + "B";
    }

    if (dx != 0) {
      set_cursor_position += "\x1B[" + std::to_string(dx) + "D";
      reset_cursor_position += "\x1B[" + std::to_string(dx) + "C";
    }

    if (cursor_.shape == Cursor::Hidden) {
      set_cursor_position += "\033[?25l";
    } else {
      set_cursor_position += "\033[?25h";
      set_cursor_position +=
          "\033[" + std::to_string(int(cursor_.shape)) + " q";
    }
  }

  std::cout << ToString() << set_cursor_position;
  Flush();
  Clear();
  frame_valid_ = true;
}

// private
void ScreenInteractive::ResetCursorPosition() {
  std::cout << reset_cursor_position;
  reset_cursor_position = "";
}

/// @brief Return a function to exit the main loop.
/// @ingroup component
Closure ScreenInteractive::ExitLoopClosure() {
  return [this] { Exit(); };
}

/// @brief Exit the main loop.
/// @ingroup component
void ScreenInteractive::Exit() {
  Post([this] { ExitNow(); });
}

// private:
void ScreenInteractive::ExitNow() {
  quit_ = true;
  task_sender_.reset();
}

// private:
void ScreenInteractive::Signal(int signal) {
  if (signal == SIGABRT) {
    Exit();
    return;
  }

// Windows do no support SIGTSTP / SIGWINCH
#if !defined(_WIN32)
  if (signal == SIGTSTP) {
    Post([&] {
      ResetCursorPosition();
      std::cout << ResetPosition(/*clear*/ true);  // Cursor to the beginning
      Uninstall();
      dimx_ = 0;
      dimy_ = 0;
      Flush();
      std::ignore = std::raise(SIGTSTP);
      Install();
    });
    return;
  }

  if (signal == SIGWINCH) {
    Post(Event::Special({0}));
    return;
  }
#endif
}

}  // namespace ftxui.
