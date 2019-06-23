#include "ftxui/component/screen_interactive.hpp"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include "ftxui/component/component.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"

namespace ftxui {

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
  //std::cout << "\033[?9h";    [> Send Mouse Row & Column on Button Press <]
  //std::cout << "\033[?1000h"; [> Send Mouse X & Y on button press and release <]
  //std::cout << std::flush;

  // Save the old terminal configuration.
  struct termios terminal_configuration_old;
  tcgetattr(STDIN_FILENO, &terminal_configuration_old);

  // Set the new terminal configuration
  struct termios terminal_configuration_new;
  terminal_configuration_new = terminal_configuration_old;

  // Non canonique terminal.
  terminal_configuration_new.c_lflag &= ~ICANON;
  // Do not print after a key press.
  terminal_configuration_new.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration_new);

  std::thread read_char([this]() {
    while (!quit_) {
      auto event = Event::GetEvent([] { return (char)getchar(); });
      PostEvent(std::move(event));
    }
  });

  std::string reset_position;
  while (!quit_) {
    reset_position = ResetPosition();
    Draw(component);
    std::cout << reset_position << ToString() << std::flush;
    Clear();
    EventLoop(component);
  }

  // Restore the old terminal configuration.
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration_old);

  read_char.join();

  std::cout << std::endl;
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

  if (dimx != dimx_ || dimy != dimy_) {
    dimx_ = dimx;
    dimy_ = dimy;
    pixels_ = std::vector<std::vector<Pixel>>(
        dimy, std::vector<Pixel>(dimx));
  }

  Render(*this, document.get());
}

std::function<void()> ScreenInteractive::ExitLoopClosure() {
  return [this]() { quit_ = true; };
}

}  // namespace ftxui.
