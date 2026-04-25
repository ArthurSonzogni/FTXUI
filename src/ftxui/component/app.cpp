// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/app.hpp"
#include <algorithm>  // for any_of, copy, max, min
#include <array>      // for array
#include <atomic>
#include <chrono>  // for operator-, milliseconds, operator>=, duration, common_type<>::type, time_point
#include <csignal>  // for signal, SIGTSTP, SIGABRT, SIGWINCH, raise, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM, __sighandler_t, size_t
#include <cstdint>
#include <cstdio>                    // for fileno, stdin
#include <ftxui/component/task.hpp>  // for Task, Closure, AnimationTask
#include <ftxui/screen/screen.hpp>  // for Cell, Screen::Cursor, Screen, Screen::Cursor::Hidden
#include <functional>        // for function
#include <initializer_list>  // for initializer_list
#include <iostream>  // for cout, ostream, operator<<, basic_ostream, endl, flush
#include <memory>
#include <stack>  // for stack
#include <string>
#include <string_view>
#include <thread>  // for thread, sleep_for
#include <tuple>   // for _Swallow_assign, ignore
#include <type_traits>
#include <utility>  // for move, swap
#include <variant>  // for visit, variant
#include <vector>   // for vector
#include "ftxui/component/animation.hpp"  // for TimePoint, Clock, Duration, Params, RequestAnimationFrame
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse, CapturedMouseInterface
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/loop.hpp"            // for Loop
#include "ftxui/component/multi_receiver_buffer.hpp"
#include "ftxui/component/task_runner.hpp"
#include "ftxui/component/terminal_input_parser.hpp"  // for TerminalInputParser
#include "ftxui/dom/node.hpp"                         // for Node, Render
#include "ftxui/screen/cell.hpp"                      // for Cell
#include "ftxui/screen/terminal.hpp"                  // for Dimensions, Size
#include "ftxui/screen/util.hpp"                      // for util::clamp
#include "ftxui/util/autoreset.hpp"                   // for AutoReset

#if defined(_WIN32)
#define DEFINE_CONSOLEV2_PROPERTIES
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#ifndef UNICODE
#error Must be compiled in UNICODE mode
#endif
#else
#include <fcntl.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <termios.h>  // for tcsetattr, termios, tcgetattr, TCSANOW, cc_t, ECHO, ICANON, VMIN, VTIME
#include <unistd.h>  // for STDIN_FILENO, STDOUT_FILENO, read
#endif

namespace ftxui {

namespace animation {
void RequestAnimationFrame() {
  auto* screen = App::Active();
  if (screen) {
    screen->RequestAnimationFrame();
  }
}
}  // namespace animation

class ThrottledRequest {
 public:
  ThrottledRequest(App* app,
                   std::function<void()> send,
                   task::TaskRunner& task_runner)
      : app_(app), send_(std::move(send)), task_runner_(&task_runner) {}

  void Request(bool force = false) {
    if (!app_->is_stdin_a_tty_) {
      return;
    }

    if (force) {
      Send();
      return;
    }

    // Allow only one pending request at a time. This is to avoid flooding the
    // terminal with requests.
    if (HasPending()) {
      return;
    }

    const auto now = std::chrono::steady_clock::now();
    const auto delta = now - last_request_time_;
    const auto delay = std::chrono::milliseconds(500) - delta;

    if (delay <= std::chrono::milliseconds(0)) {
      Send();
      return;
    }

    request_queued_ = true;
    task_runner_->PostDelayedTask(
        [this] {
          request_queued_ = false;
          Request();
        },
        delay);
  }

  void OnReply() { pending_request_ = false; }

  bool HasPending() const {
    if (pending_request_) {
      const auto now = std::chrono::steady_clock::now();
      if (now - last_sent_time_ < std::chrono::seconds(5)) {
        return true;
      }
    }
    return request_queued_;
  }

 private:
  void Send() {
    last_sent_time_ = std::chrono::steady_clock::now();
    pending_request_ = true;
    send_();
  }

  App* app_;
  std::function<void()> send_;
  task::TaskRunner* task_runner_;
  bool pending_request_ = false;
  std::chrono::steady_clock::time_point last_request_time_ =
      std::chrono::steady_clock::now() - std::chrono::hours(1);
  std::chrono::steady_clock::time_point last_sent_time_ =
      std::chrono::steady_clock::now() - std::chrono::hours(1);
  bool request_queued_ = false;
};

struct App::Internal {
  // Convert char to Event.
  TerminalInputParser terminal_input_parser;

  task::TaskRunner task_runner;

  // The last time a character was received.
  std::chrono::time_point<std::chrono::steady_clock> last_char_time =
      std::chrono::steady_clock::now();

  // The buffer used to output the screen to the terminal.
  // Unlike for std::vector::clear, the C++ standard does not explicitly require
  // that capacity is unchanged by this function, but existing implementations
  // do not change capacity. This means that they do not release the allocated
  // memory (see also shrink_to_fit).
  std::string output_buffer;

  ThrottledRequest cursor_position_request;

  MultiReceiverBuffer<Event> event_buffer;
  std::unique_ptr<MultiReceiverBuffer<Event>::Receiver> setup_receiver;
  std::unique_ptr<MultiReceiverBuffer<Event>::Receiver> main_loop_receiver;

  explicit Internal(App* app, std::function<void(Event)> out);
};

namespace {

App* g_active_screen = nullptr;  // NOLINT

std::stack<Closure> on_exit_functions;  // NOLINT

void OnExit() {
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }
}

#if defined(_WIN32)

#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>

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

#endif

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
    App::Private::Signal(*g_active_screen, SIGABRT);
  }

#if !defined(_WIN32)
  int signal_stop_count = g_signal_stop_count.exchange(0);
  while (signal_stop_count--) {
    App::Private::Signal(*g_active_screen, SIGTSTP);
  }

  int signal_resize_count = g_signal_resize_count.exchange(0);
  while (signal_resize_count--) {
    App::Private::Signal(*g_active_screen, SIGWINCH);
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

}  // namespace

App::Internal::Internal(App* app, std::function<void(Event)> out)
    : terminal_input_parser(std::move(out)),
      cursor_position_request(
          app,
          [app] { app->TerminalSend(DeviceStatusReport(DSRMode::kCursor)); },
          task_runner) {
  setup_receiver = event_buffer.CreateReceiver();
  main_loop_receiver = event_buffer.CreateReceiver();
}

App::App(Dimension dimension, int dimx, int dimy, bool use_alternative_screen)
    : Screen(dimx, dimy),
      dimension_(dimension),
      use_alternative_screen_(use_alternative_screen) {
  internal_ = std::make_unique<Internal>(this, [&](Event event) {
    internal_->event_buffer.Push(std::move(event));
    RequestAnimationFrame();
  });
}

// static
App App::FixedSize(int dimx, int dimy) {
  return {
      Dimension::Fixed,
      dimx,
      dimy,
      /*use_alternative_screen=*/false,
  };
}

/// Create a App taking the full terminal size. This is using the
/// alternate screen buffer to avoid messing with the terminal content.
/// @note This is the same as `App::FullscreenAlternateScreen()`
// static
App App::Fullscreen() {
  return FullscreenAlternateScreen();
}

/// Create a App taking the full terminal size. The primary screen
/// buffer is being used. It means if the terminal is resized, the previous
/// content might mess up with the terminal content.
// static
App App::FullscreenPrimaryScreen() {
  auto terminal = Terminal::Size();
  return {
      Dimension::Fullscreen,
      terminal.dimx,
      terminal.dimy,
      /*use_alternative_screen=*/false,
  };
}

/// Create a App taking the full terminal size. This is using the
/// alternate screen buffer to avoid messing with the terminal content.
// static
App App::FullscreenAlternateScreen() {
  auto terminal = Terminal::Size();
  return {
      Dimension::Fullscreen,
      terminal.dimx,
      terminal.dimy,
      /*use_alternative_screen=*/true,
  };
}

/// Create a App whose width match the terminal output width and
/// the height matches the component being drawn.
// static
App App::TerminalOutput() {
  auto terminal = Terminal::Size();
  return {
      Dimension::TerminalOutput,
      terminal.dimx,
      terminal.dimy,  // Best guess.
      /*use_alternative_screen=*/false,
  };
}

App::~App() = default;

/// Create a App whose width and height match the component being
/// drawn.
// static
App App::FitComponent() {
  auto terminal = Terminal::Size();
  return {
      Dimension::FitComponent,
      terminal.dimx,  // Best guess.
      terminal.dimy,  // Best guess.
      false,
  };
}

/// @brief Set whether mouse is tracked and events reported.
/// called outside of the main loop. E.g `App::Loop(...)`.
/// @param enable Whether to enable mouse event tracking.
/// @note This muse be called outside of the main loop. E.g. before calling
/// `App::Loop`.
/// @note Mouse tracking is enabled by default.
/// @note Mouse tracking is only supported on terminals that supports it.
///
/// ### Example
///
/// ```cpp
/// auto screen = App::TerminalOutput();
/// screen.TrackMouse(false);
/// screen.Loop(component);
/// ```
void App::TrackMouse(bool enable) {
  track_mouse_ = enable;
}

/// @brief Enable or disable automatic piped input handling.
/// When enabled, FTXUI will detect piped input and redirect stdin from /dev/tty
/// for keyboard input, allowing applications to read piped data while still
/// receiving interactive keyboard events.
/// @param enable Whether to enable piped input handling. Default is true.
/// @note This must be called before Loop().
/// @note This feature is enabled by default.
/// @note This feature is only available on POSIX systems (Linux/macOS).
void App::HandlePipedInput(bool enable) {
  handle_piped_input_ = enable;
}

/// @brief Add a task to the main loop.
/// It will be executed later, after every other scheduled tasks.

void App::PostEventOrExecute(Closure closure) {
  if (!closure) {
    return;
  }
  if (auto* app = App::Active()) {
    app->Post(std::move(closure));
  } else {
    closure();
  }
}

void App::Post(Task task) {
  internal_->task_runner.PostTask([this, task = std::move(task)]() mutable {
    if (component_) {
      HandleTask(component_, task);
      return;
    }

    // If there is no component, we can still execute closures.
    if (std::holds_alternative<Closure>(task)) {
      std::get<Closure>(task)();
    }
  });
}

/// @brief Add an event to the main loop.
/// It will be executed later, after every other scheduled events.
void App::PostEvent(Event event) {
  internal_->event_buffer.Push(std::move(event));
  RequestAnimationFrame();
}

/// @brief Add a task to draw the screen one more time, until all the animations
/// are done.
void App::RequestAnimationFrame() {
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

/// @brief Try to get the unique lock about being able to capture the mouse.
/// @return A unique lock if the mouse is not already captured, otherwise a
/// null.
CapturedMouse App::CaptureMouse() {
  if (mouse_captured) {
    return nullptr;
  }
  mouse_captured = true;
  return std::make_unique<CapturedMouseImpl>(
      [this] { mouse_captured = false; });
}

/// @brief Execute the main loop.
/// @param component The component to draw.
void App::Loop(Component component) {  // NOLINT
  class Loop loop(this, std::move(component));
  loop.Run();
}

/// @brief Return whether the main loop has been quit.
bool App::HasQuitted() {
  return quit_;
}

// private
void App::PreMain() {
  // Suspend previously active screen:
  if (g_active_screen) {
    std::swap(suspended_screen_, g_active_screen);
    // Reset cursor position to the top of the screen and clear the screen.
    suspended_screen_->TerminalSend(suspended_screen_->ResetCursorPosition());
    suspended_screen_->ResetPosition(
        suspended_screen_->internal_->output_buffer,
        /*clear=*/true);
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
void App::PostMain() {
  // Put cursor position at the end of the drawing.
  TerminalSend(ResetCursorPosition());

  g_active_screen = nullptr;

  // Restore suspended screen.
  if (suspended_screen_) {
    // Clear screen, and put the cursor at the beginning of the drawing.
    ResetPosition(internal_->output_buffer, /*clear=*/true);
    dimx_ = 0;
    dimy_ = 0;
    Uninstall();
    std::swap(g_active_screen, suspended_screen_);
    g_active_screen->Install();
  } else {
    Uninstall();

    std::cout << "\r";
    // On final exit, keep the current drawing and reset cursor position one
    // line after it.
    if (!use_alternative_screen_) {
      std::cout << "\n";
    }
    std::cout << std::flush;
  }
}

/// @brief Decorate a function. It executes the same way, but with the currently
/// active screen terminal hooks temporarily uninstalled during its execution.
/// @param fn The function to decorate.
Closure App::WithRestoredIO(Closure fn) {  // NOLINT
  return [this, fn] {
    Uninstall();
    fn();
    Install();
  };
}

/// @brief Force FTXUI to handle or not handle Ctrl-C, even if the component
/// catches the Event::CtrlC.
void App::ForceHandleCtrlC(bool force) {
  force_handle_ctrl_c_ = force;
}

/// @brief Force FTXUI to handle or not handle Ctrl-Z, even if the component
/// catches the Event::CtrlZ.
void App::ForceHandleCtrlZ(bool force) {
  force_handle_ctrl_z_ = force;
}

/// @brief Returns the content of the current selection
std::string App::GetSelection() {
  if (!selection_) {
    return "";
  }
  return selection_->GetParts();
}

void App::SelectionChange(std::function<void()> callback) {
  selection_on_change_ = std::move(callback);
}

/// @brief Return the currently active screen, or null if none.
// static
App* App::Active() {
  return g_active_screen;
}

// private
void App::Install() {
  frame_valid_ = false;

  // Flush the buffer for stdout to ensure whatever the user has printed before
  // is fully applied before we start modifying the terminal configuration. This
  // is important, because we are using two different channels (stdout vs
  // termios/WinAPI) to communicate with the terminal emulator below. See
  // https://github.com/ArthurSonzogni/FTXUI/issues/846
  TerminalFlush();

  InstallPipedInputHandling();

  // After uninstalling the new configuration, flush it to the terminal to
  // ensure it is fully applied:
  on_exit_functions.emplace([this] { TerminalFlush(); });

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
#else  // POSIX (Linux & Mac)
  // #if defined(__EMSCRIPTEN__)
  //// Reading stdin isn't blocking.
  // int flags = fcntl(0, F_GETFL, 0);
  // fcntl(0, F_SETFL, flags | O_NONBLOCK);

  //// Restore the terminal configuration on exit.
  // on_exit_functions.emplace([flags] { fcntl(0, F_SETFL, flags); });
  // #endif
  for (const int signal : {SIGWINCH, SIGTSTP}) {
    InstallSignalHandler(signal);
  }

  struct termios terminal;  // NOLINT
  tcgetattr(tty_fd_, &terminal);
  on_exit_functions.emplace([terminal = terminal, tty_fd_ = tty_fd_] {
    tcsetattr(tty_fd_, TCSANOW, &terminal);
  });

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

  tcsetattr(tty_fd_, TCSANOW, &terminal);

#endif

  auto enable = [&](const std::vector<DECMode>& parameters) {
    TerminalSend(Set(parameters));
    on_exit_functions.emplace(
        [this, parameters] { TerminalSend(Reset(parameters)); });
  };

  auto disable = [&](const std::vector<DECMode>& parameters) {
    TerminalSend(Reset(parameters));
    on_exit_functions.emplace(
        [this, parameters] { TerminalSend(Set(parameters)); });
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
  TerminalFlush();

  InstallTerminalInfo();

  quit_ = false;

  PostAnimationTask();

  installed_ = true;
}

void App::InstallPipedInputHandling() {
  is_stdin_a_tty_ = false;
  is_stdout_a_tty_ = false;
#if defined(__EMSCRIPTEN__)
  is_stdin_a_tty_ = true;
  is_stdout_a_tty_ = true;
#elif defined(_WIN32)
  is_stdin_a_tty_ = _isatty(_fileno(stdin));
  is_stdout_a_tty_ = _isatty(_fileno(stdout));
#else
  tty_fd_ = STDIN_FILENO;
  is_stdout_a_tty_ = isatty(STDOUT_FILENO);
  // Handle piped input redirection if explicitly enabled by the application.
  // This allows applications to read data from stdin while still receiving
  // keyboard input from the terminal for interactive use.
  if (!handle_piped_input_) {
    is_stdin_a_tty_ = isatty(STDIN_FILENO);
  } else if (isatty(STDIN_FILENO)) {
    is_stdin_a_tty_ = true;
  } else {
    // Open /dev/tty for keyboard input.
    tty_fd_ = open("/dev/tty", O_RDONLY);  // NOLINT
    if (tty_fd_ < 0) {
      // Failed to open /dev/tty (containers, headless systems, etc.)
      tty_fd_ = STDIN_FILENO;  // Fallback to stdin.
      is_stdin_a_tty_ = isatty(STDIN_FILENO);
    } else {
      is_stdin_a_tty_ = true;
      // Close the /dev/tty file descriptor on exit.
      on_exit_functions.emplace([this] {
        close(tty_fd_);
        tty_fd_ = -1;
      });
    }
  }
#endif
}

/// @brief Return the names of the terminal capabilities.
std::vector<std::string> App::TerminalCapabilityNames() const {
  return Event::TerminalCapabilities("", terminal_capabilities_)
      .TerminalCapabilityNames();
}

/// @brief Return the terminal name.
const std::string& App::TerminalName() const {
  return terminal_name_;
}

/// @brief Return the terminal version.
int App::TerminalVersion() const {
  return terminal_version_;
}

/// @brief Return the terminal emulator name.
const std::string& App::TerminalEmulatorName() const {
  return terminal_emulator_name_;
}

/// @brief Return the terminal emulator version.
const std::string& App::TerminalEmulatorVersion() const {
  return terminal_emulator_version_;
}

/// @brief Return the terminal capabilities.
const std::vector<int>& App::TerminalCapabilities() const {
  return terminal_capabilities_;
}

// NOLINTNEXTLINE
void App::InstallTerminalInfo() {
  // Request the terminal to report the current cursor shape. We will restore it
  // on exit.
  if (is_stdout_a_tty_) {
    TerminalSend(DECRQSS_DECSCUSR);
    TerminalSend("\033[c");   // DA1
    TerminalSend("\033[>c");  // DA2
    TerminalSend("\033[>q");  // XTVERSION
    TerminalFlush();
  }

  int cursor_reset_shape = 1;

  // Wait for the cursor shape reply using the setup head.
  if (is_stdin_a_tty_ && is_stdout_a_tty_) {
    auto start = std::chrono::steady_clock::now();
    bool cursor_shape_received = false;
    bool da1_received = false;
    bool da2_received = false;
    bool xtversion_received = false;
    // Wait for the cursor shape reply using the setup head.
    while (true) {
      FetchTerminalEvents();
      while (internal_->setup_receiver->Has()) {
        const auto event = internal_->setup_receiver->Pop();
        if (event.is_cursor_shape()) {
          cursor_reset_shape = event.cursor_shape();
          cursor_shape_received = true;
        }
        if (event.IsTerminalCapabilities()) {
          terminal_capabilities_ = event.TerminalCapabilities();
          da1_received = true;
        }
        if (event.IsTerminalNameVersion()) {
          terminal_name_ = event.TerminalName();
          terminal_version_ = event.TerminalVersion();
          da2_received = true;
        }
        if (event.IsTerminalEmulator()) {
          terminal_emulator_name_ = event.TerminalEmulatorName();
          terminal_emulator_version_ = event.TerminalEmulatorVersion();
          xtversion_received = true;
        }
      }

      if (cursor_shape_received && da1_received && da2_received) {
        break;
      }

      if (std::chrono::steady_clock::now() - start >
          std::chrono::milliseconds(1000)) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // If we received DA1, DA2 and cursor shape, but not XTVERSION yet,
    // wait a tiny bit more for XTVERSION as it is usually sent last.
    if (!xtversion_received && cursor_shape_received && da1_received &&
        da2_received) {
      for (int i = 0; i < 10; ++i) {
        FetchTerminalEvents();
        while (internal_->setup_receiver->Has()) {
          const auto event = internal_->setup_receiver->Pop();
          if (event.IsTerminalEmulator()) {
            terminal_emulator_name_ = event.TerminalEmulatorName();
            terminal_emulator_version_ = event.TerminalEmulatorVersion();
            xtversion_received = true;
          }
        }
        if (xtversion_received) {
          break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }

  // Set quirks and color support based on terminal identification.
  Terminal::Quirks quirks = Terminal::GetQuirks();

  const bool is_modern_emulator = (TerminalEmulatorName() != "unknown");
  const bool is_urxvt = (TerminalName() == "urxvt");
  const bool is_vt220_plus =
      (TerminalName() != "vt100" && TerminalName() != "unknown");
  bool reports_utf8 = false;
  for (const int x : terminal_capabilities_) {
    if (x == 42) {
      reports_utf8 = true;
      break;
    }
  }
  bool reports_color = false;
  for (const int x : terminal_capabilities_) {
    if (x == 22) {
      reports_color = true;
      break;
    }
  }

  // Heuristic:
  // 1. If it's a modern emulator, we can trust it supports TrueColor.
  if (is_modern_emulator) {
    quirks.color_support =
        std::max(quirks.color_support, Terminal::Color::TrueColor);
    quirks.block_characters = true;
    quirks.cursor_hiding = true;
    quirks.component_ascii = false;
  } else if (is_vt220_plus || reports_utf8 || reports_color) {
    // For VT220+ (including urxvt) or terminals reporting color support:
    // - Enable modern quirks.
    // - Ensure at least 256 colors.
    // - If it's NOT urxvt, we can also upgrade to TrueColor as most modern
    //   terminals identifying as VT220 support it.
    quirks.block_characters = true;
    quirks.cursor_hiding = true;
    quirks.component_ascii = false;
    if (!is_urxvt && quirks.color_support < Terminal::Color::TrueColor) {
      quirks.color_support = Terminal::Color::TrueColor;
    } else if (quirks.color_support < Terminal::Color::Palette256) {
      quirks.color_support = Terminal::Color::Palette256;
    }
  }

  Terminal::SetQuirks(quirks);

  on_exit_functions.emplace([this, cursor_reset_shape] {
    TerminalSend("\033[?25h");  // Enable cursor.
    if (is_stdout_a_tty_) {
      TerminalSend("\033[" + std::to_string(cursor_reset_shape) + " q");
    }
  });
}

// private
void App::Uninstall() {
  installed_ = false;

  // During shutdown, wait for all of the replies.
  if (is_stdin_a_tty_ && is_stdout_a_tty_) {
    auto closing_receiver = internal_->event_buffer.CreateReceiverAt(
        internal_->main_loop_receiver->index());
    auto start = std::chrono::steady_clock::now();
    while (internal_->cursor_position_request.HasPending()) {
      FetchTerminalEvents();

      while (closing_receiver->Has()) {
        const auto event = closing_receiver->Pop();
        if (event.is_cursor_position()) {
          cursor_x_ = event.cursor_x();
          cursor_y_ = event.cursor_y();
          internal_->cursor_position_request.OnReply();
        }
      }

      internal_->task_runner.RunUntilIdle();

      if (std::chrono::steady_clock::now() - start >
          std::chrono::milliseconds(400)) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  OnExit();
}

// private
// NOLINTNEXTLINE
void App::RunOnceBlocking(Component component) {
  // Set FPS to 60 at most.
  const auto time_per_frame = std::chrono::microseconds(16666);  // 1s / 60fps

  auto time = std::chrono::steady_clock::now();
  const size_t executed_task = internal_->task_runner.ExecutedTasks();

  // Wait for at least one task to execute.
  while (executed_task == internal_->task_runner.ExecutedTasks() &&
         !HasQuitted()) {
    RunOnce(component);

    const auto now = std::chrono::steady_clock::now();
    const auto delta = now - time;
    time = now;

    if (delta < time_per_frame) {
      const auto sleep_duration = time_per_frame - delta;
      std::this_thread::sleep_for(sleep_duration);
    }
  }
}

// private
void App::RunOnce(const Component& component) {
  const AutoReset set_component(&component_, component);
  ExecuteSignalHandlers();
  FetchTerminalEvents();

  while (!quit_ && internal_->main_loop_receiver->Has()) {
    Post(internal_->main_loop_receiver->Pop());
  }

  // Execute the pending tasks from the queue.
  const size_t executed_task = internal_->task_runner.ExecutedTasks();
  internal_->task_runner.RunUntilIdle();
  // If no executed task, we can return early without redrawing the screen.
  if (executed_task == internal_->task_runner.ExecutedTasks()) {
    return;
  }

  ExecuteSignalHandlers();
  Draw(component);

  if (selection_data_previous_ != selection_data_) {
    selection_data_previous_ = selection_data_;
    if (selection_on_change_) {
      selection_on_change_();
      Post(Event::Custom);
    }
  }
}

// private
// NOLINTNEXTLINE
void App::HandleTask(Component component, Task& task) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        // clang-format off

    // Handle Event.
    if constexpr (std::is_same_v<T, Event>) {

      if (arg.is_cursor_position()) {
        cursor_x_ = arg.cursor_x();
        cursor_y_ = arg.cursor_y();
        internal_->cursor_position_request.OnReply();
        return;
      }



      if (arg.is_mouse()) {
        arg.mouse().x -= cursor_x_;
        arg.mouse().y -= cursor_y_;
      }

      arg.screen_ = this;

      bool handled = component->OnEvent(arg);
      handled = HandleSelection(handled, arg);

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
bool App::HandleSelection(bool handled, Event event) {
  if (handled) {
    selection_pending_ = nullptr;
    selection_data_.empty = true;
    selection_ = nullptr;
    return true;
  }

  if (!event.is_mouse()) {
    return false;
  }

  auto& mouse = event.mouse();
  if (mouse.button != Mouse::Left) {
    return false;
  }

  if (mouse.motion == Mouse::Pressed) {
    selection_pending_ = CaptureMouse();
    selection_data_.start_x = mouse.x;
    selection_data_.start_y = mouse.y;
    selection_data_.end_x = mouse.x;
    selection_data_.end_y = mouse.y;
    return false;
  }

  if (!selection_pending_) {
    return false;
  }

  if (mouse.motion == Mouse::Moved) {
    if ((mouse.x != selection_data_.end_x) ||
        (mouse.y != selection_data_.end_y)) {
      selection_data_.end_x = mouse.x;
      selection_data_.end_y = mouse.y;
      selection_data_.empty = false;
    }

    return true;
  }

  if (mouse.motion == Mouse::Released) {
    selection_pending_ = nullptr;
    selection_data_.end_x = mouse.x;
    selection_data_.end_y = mouse.y;
    selection_data_.empty = false;
    return true;
  }

  return false;
}

// private
// NOLINTNEXTLINE
void App::Draw(Component component) {
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
      dimy = util::clamp(document->requirement().min_y, 0, terminal.dimy);
      break;
    case Dimension::Fullscreen:
      dimx = terminal.dimx;
      dimy = terminal.dimy;
      break;
    case Dimension::FitComponent:
      dimx = util::clamp(document->requirement().min_x, 0, terminal.dimx);
      dimy = util::clamp(document->requirement().min_y, 0, terminal.dimy);
      break;
  }

  // Hide cursor to prevent flickering during reset.
  TerminalSend("\033[?25l");

  const bool resized = frame_count_ == 0 || (dimx != dimx_) || (dimy != dimy_);
  TerminalSend(ResetCursorPosition());

  if (frame_count_ != 0) {
    // Reset the cursor position to the lower left corner to start drawing the
    // new frame.
    ResetPosition(internal_->output_buffer, resized);

    // If the terminal width decrease, the terminal emulator will start wrapping
    // lines and make the display dirty. We should clear it completely.
    if ((dimx < dimx_) && !use_alternative_screen_) {
      TerminalSend("\033[J");  // clear terminal output
      TerminalSend("\033[H");  // move cursor to home position
    }
  }

  // Resize the screen if needed.
  if (resized) {
    dimx_ = dimx;
    dimy_ = dimy;
    cells_ = std::vector<std::vector<Cell>>(dimy, std::vector<Cell>(dimx));
    cursor_.x = dimx_ - 1;
    cursor_.y = dimy_ - 1;
  }

  // Periodically request the terminal emulator the frame position relative to
  // the screen. This is useful for converting mouse position reported in
  // screen's coordinates to frame's coordinates.
  if (!use_alternative_screen_ && is_stdout_a_tty_) {
    RequestCursorPosition(previous_frame_resized_);
  }
  previous_frame_resized_ = resized;

  selection_ = selection_data_.empty
                   ? std::make_unique<Selection>()
                   : std::make_unique<Selection>(
                         selection_data_.start_x, selection_data_.start_y,  //
                         selection_data_.end_x, selection_data_.end_y);
  Render(*this, document.get(), *selection_);

  // Set cursor position for user using tools to insert CJK characters.
  {
    const int dx = dimx_ - 1 - cursor_.x + int(dimx_ != terminal.dimx);
    const int dy = dimy_ - 1 - cursor_.y;

    set_cursor_position_.clear();
    reset_cursor_position_.clear();

    if (dy != 0) {
      set_cursor_position_ += "\x1B[" + std::to_string(dy) + "A";
      reset_cursor_position_ += "\x1B[" + std::to_string(dy) + "B";
    }

    if (dx != 0) {
      set_cursor_position_ += "\x1B[" + std::to_string(dx) + "D";
      reset_cursor_position_ += "\x1B[" + std::to_string(dx) + "C";
    }

    if (cursor_.shape != Cursor::Hidden) {
      set_cursor_position_ += "\033[?25h";
      set_cursor_position_ +=
          "\033[" + std::to_string(int(cursor_.shape)) + " q";
    }
  }

  ToString(internal_->output_buffer);
  TerminalSend(set_cursor_position_);
  TerminalFlush();

  Clear();
  frame_valid_ = true;
  frame_count_++;
}

// private
std::string App::ResetCursorPosition() {
  std::string result = std::move(reset_cursor_position_);
  reset_cursor_position_ = "";
  return result;
}

// private
void App::RequestCursorPosition(bool force) {
  internal_->cursor_position_request.Request(force);
}

// private

// private
void App::TerminalSend(std::string_view s) {
  internal_->output_buffer += s;
}

// private
void App::TerminalFlush() {
  // Emscripten doesn't implement flush. We interpret zero as flush.
  internal_->output_buffer += '\0';
  std::cout << internal_->output_buffer << std::flush;
  internal_->output_buffer.clear();
}

/// @brief Return a function to exit the main loop.
Closure App::ExitLoopClosure() {
  return [this] { Exit(); };
}

/// @brief Exit the main loop.
void App::Exit() {
  Post([this] { ExitNow(); });
}

// private:
void App::ExitNow() {
  quit_ = true;
}

// private:
void App::Signal(int signal) {
  if (signal == SIGABRT) {
    Exit();
    return;
  }

// Windows do no support SIGTSTP / SIGWINCH
#if !defined(_WIN32)
  if (signal == SIGTSTP) {
    Post([&] {
      TerminalSend(ResetCursorPosition());
      ResetPosition(internal_->output_buffer, /*clear*/ true);
      Uninstall();
      dimx_ = 0;
      dimy_ = 0;
      (void)std::raise(SIGTSTP);
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

size_t App::FetchTerminalEvents() {
#if defined(_WIN32)
  auto get_input_records = [&]() -> std::vector<INPUT_RECORD> {
    // Check if there is input in the console.
    auto console = GetStdHandle(STD_INPUT_HANDLE);
    DWORD number_of_events = 0;
    if (!GetNumberOfConsoleInputEvents(console, &number_of_events)) {
      return std::vector<INPUT_RECORD>();
    }
    if (number_of_events <= 0) {
      // No input, return.
      return std::vector<INPUT_RECORD>();
    }
    // Read the input events.
    std::vector<INPUT_RECORD> records(number_of_events);
    DWORD number_of_events_read = 0;
    if (!ReadConsoleInput(console, records.data(), (DWORD)records.size(),
                          &number_of_events_read)) {
      return std::vector<INPUT_RECORD>();
    }
    records.resize(number_of_events_read);
    return records;
  };

  auto records = get_input_records();
  if (records.size() == 0) {
    const auto timeout =
        std::chrono::steady_clock::now() - internal_->last_char_time;
    const size_t timeout_microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(timeout).count();
    internal_->terminal_input_parser.Timeout(timeout_microseconds);
    return 0;
  }
  internal_->last_char_time = std::chrono::steady_clock::now();

  // Convert the input events to FTXUI events.
  // For each event, we call the terminal input parser to convert it to
  // Event.
  std::wstring wstring;
  for (const auto& r : records) {
    switch (r.EventType) {
      case KEY_EVENT: {
        auto key_event = r.Event.KeyEvent;
        // ignore UP key events
        if (key_event.bKeyDown == FALSE) {
          continue;
        }
        const wchar_t wc = key_event.uChar.UnicodeChar;
        wstring += wc;
        if (wc >= 0xd800 && wc <= 0xdbff) {
          // Wait for the Low Surrogate to arrive in the next record.
          continue;
        }
        for (auto it : to_string(wstring)) {
          internal_->terminal_input_parser.Add(it);
        }
        wstring.clear();
      } break;
      case WINDOW_BUFFER_SIZE_EVENT:
        Post(Event::Special({0}));
        break;
      case MENU_EVENT:
      case FOCUS_EVENT:
      case MOUSE_EVENT:
        // TODO(mauve): Implement later.
        break;
    }
  }
  return records.size();
#elif defined(__EMSCRIPTEN__)
  // Read chars from the terminal.
  // We configured it to be non blocking.
  std::array<char, 128> out{};
  const ssize_t l = read(STDIN_FILENO, out.data(), out.size());
  if (l <= 0) {
    const auto timeout =
        std::chrono::steady_clock::now() - internal_->last_char_time;
    const size_t timeout_microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(timeout).count();
    internal_->terminal_input_parser.Timeout(timeout_microseconds);
    return 0;
  }
  internal_->last_char_time = std::chrono::steady_clock::now();

  // Convert the chars to events.
  for (ssize_t i = 0; i < l; ++i) {
    internal_->terminal_input_parser.Add(out.at(static_cast<size_t>(i)));
  }
  return (size_t)l;
#else  // POSIX (Linux & Mac)
  struct pollfd pfd = {tty_fd_, POLLIN, 0};
  const int poll_result = poll(&pfd, 1, 0);
  if (poll_result <= 0) {
    const auto timeout =
        std::chrono::steady_clock::now() - internal_->last_char_time;
    const size_t timeout_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    internal_->terminal_input_parser.Timeout(static_cast<int>(timeout_ms));
    return 0;
  }
  internal_->last_char_time = std::chrono::steady_clock::now();

  // Read chars from the terminal.
  std::array<char, 128> out{};
  const ssize_t l = read(tty_fd_, out.data(), out.size());
  if (l <= 0) {
    return 0;
  }

  // Convert the chars to events.
  for (ssize_t i = 0; i < l; ++i) {
    internal_->terminal_input_parser.Add(out.at(static_cast<size_t>(i)));
  }
  return (size_t)l;
#endif
}

void App::PostAnimationTask() {
  Post(AnimationTask());

  // Repeat the animation task every 15ms. This correspond to a frame rate
  // of around 66fps.
  internal_->task_runner.PostDelayedTask([this] { PostAnimationTask(); },
                                         std::chrono::milliseconds(15));
}

bool App::SelectionData::operator==(const App::SelectionData& other) const {
  if (empty && other.empty) {
    return true;
  }
  if (empty || other.empty) {
    return false;
  }
  return start_x == other.start_x && start_y == other.start_y &&
         end_x == other.end_x && end_y == other.end_y;
}

bool App::SelectionData::operator!=(const App::SelectionData& other) const {
  return !(*this == other);
}

}  // namespace ftxui
