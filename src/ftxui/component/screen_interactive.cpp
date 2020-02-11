#include "ftxui/component/screen_interactive.hpp"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <stack>
#include <thread>
#include "ftxui/component/component.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"

#if defined(__clang__) && defined (__APPLE__)
    // Quick exit is missing in standard CLang headers
    #define quick_exit(a) exit(a)
#endif

namespace ftxui {

static const char* HIDE_CURSOR = "\x1B[?25l";
static const char* SHOW_CURSOR = "\x1B[?25h";

static const char* DISABLE_LINE_WRAP = "\x1B[7l";
static const char* ENABLE_LINE_WRAP = "\x1B[7h";

std::stack<std::function<void()>> on_exit_functions;
void OnExit(int signal) {
  while (!on_exit_functions.empty()) {
    on_exit_functions.top()();
    on_exit_functions.pop();
  }

  if (signal == SIGINT)
    quick_exit(SIGINT);
}

std::function<void()> on_resize = []{};
void OnResize(int /* signal */) {
  on_resize();
}

ScreenInteractive::ScreenInteractive(int dimx,
                                     int dimy,
                                     Dimension dimension)
    : Screen(dimx, dimy), dimension_(dimension) {}
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
  std::unique_lock<std::mutex> lock(events_queue_mutex);
  events_queue.push(event);
  events_queue_cv.notify_one();
}

void ScreenInteractive::EventLoop(Component* component) {
  std::unique_lock<std::mutex> lock(events_queue_mutex);
  while (!quit_ && events_queue.empty())
    events_queue_cv.wait(lock);

  // After the wait, we own the lock.
  while (!events_queue.empty()) {
    component->OnEvent(events_queue.front());
    events_queue.pop();
  }
}

void ScreenInteractive::Loop(Component* component) {
  // Install a SIGINT handler and restore the old handler on exit.
  auto old_sigint_handler = std::signal(SIGINT, OnExit);
  on_exit_functions.push(
      [old_sigint_handler]() { std::signal(SIGINT, old_sigint_handler); });

  // Save the old terminal configuration and restore it on exit.
  struct termios terminal_configuration_old;
  tcgetattr(STDIN_FILENO, &terminal_configuration_old);
  on_exit_functions.push(
      [terminal_configuration_old = terminal_configuration_old]() {
        tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration_old);
      });

  // Set the new terminal configuration
  struct termios terminal_configuration_new;
  terminal_configuration_new = terminal_configuration_old;

  // Non canonique terminal.
  terminal_configuration_new.c_lflag &= ~ICANON;
  // Do not print after a key press.
  terminal_configuration_new.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration_new);

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

  // Handle resize.
  on_resize = [&] { PostEvent(Event::Special({0})); };
  auto old_sigwinch_handler = std::signal(SIGWINCH, OnResize);
  on_exit_functions.push([&] { std::signal(SIGWINCH, old_sigwinch_handler); });

  std::thread read_char([this] {
    while (!quit_) {
      auto event = Event::GetEvent([] { return (char)getchar(); });
      PostEvent(std::move(event));
    }
  });

  while (!quit_) {
    std::cout << reset_cursor_position << ResetPosition();
    Draw(component);
    std::cout << ToString() << set_cursor_position << std::flush;
    Clear();
    EventLoop(component);
  }
  read_char.join();
  OnExit(0);
}

void ScreenInteractive::Draw(Component* component) {
  auto document = component->Render();
  int dimx;
  int dimy;
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
    pixels_ = std::vector<std::vector<Pixel>>(
        dimy, std::vector<Pixel>(dimx));
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
    set_cursor_position +=  "\x1B[" + std::to_string(dy) + "A";
    reset_cursor_position += "\x1B[" + std::to_string(dy) + "B";
  }
}

std::function<void()> ScreenInteractive::ExitLoopClosure() {
  return [this]() { quit_ = true; };
}

}  // namespace ftxui.
