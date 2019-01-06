#include "ftxui/component/screen_interactive.hpp"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include "ftxui/component/component.hpp"
#include "ftxui/component/delegate.hpp"
#include "ftxui/terminal.hpp"

namespace ftxui::component {

namespace {
constexpr int ESC = 27;
constexpr int WAT = 195;
constexpr int WAT2 = 194;
constexpr int WATWAIT = 91;

Event GetEvent() {
  int v1 = getchar();
  if (v1 == ESC) {
    int v2 = getchar();
    int v3 = getchar();

    // if (v2 == WATWAIT) {
    // int v4 = getchar();
    // int v5 = getchar();
    // return Event{v1, v2, v3, v4, v5};
    //}
    return Event{v1, v2, v3};
  }

  if (v1 == WAT) {
    int v2 = getchar();
    return Event{v1, v2};
  }

  if (v1 == WAT2) {
    int v2 = getchar();
    return Event{v1, v2};
  }

  return Event{v1};
};
};  // namespace

class ScreenInteractive::Delegate : public component::Delegate {
 public:
  Delegate() : root_(this) {}

  void Register(component::Component* c) override { component_ = c; }

  std::vector<std::unique_ptr<Delegate>> child_;
  Delegate* NewChild() override {
    Delegate* child = new Delegate;
    child->root_ = root_;
    child->parent_ = this;

    if (!child_.empty()) {
      child_.back()->next_sibling_ = child;
      child->previous_sibling_ = child_.back().get();
    }

    child_.emplace_back(child);
    return child;
  }

  void OnEvent(Event event) { component_->OnEvent(event); }

  std::vector<component::Delegate*> children() override {
    std::vector<component::Delegate*> ret;
    for (auto& it : child_)
      ret.push_back(it.get());
    return ret;
  }

  Delegate* root_;
  Delegate* parent_ = nullptr;
  Delegate* previous_sibling_ = nullptr;
  Delegate* next_sibling_ = nullptr;
  component::Component* component_;

  Delegate* Root() override { return root_; }
  Delegate* Parent() override { return parent_; }
  Delegate* PreviousSibling() override { return previous_sibling_; }
  Delegate* NextSibling() override { return next_sibling_; }
  component::Component* component() override { return component_; }
};

ScreenInteractive::ScreenInteractive(size_t dimx,
                                     size_t dimy,
                                     Dimension dimension)
    : Screen(dimx, dimy), delegate_(new Delegate), dimension_(dimension) {}
ScreenInteractive::~ScreenInteractive() {}

// static
ScreenInteractive ScreenInteractive::FixedSize(size_t dimx, size_t dimy) {
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

void ScreenInteractive::Loop() {
  std::cout << "\033[?9h";    /* Send Mouse Row & Column on Button Press */
  std::cout << "\033[?1000h"; /* Send Mouse X & Y on button press and release */
  std::cout << std::flush;

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

  std::string reset_position;
  while (!quit_) {
    PrepareDraw();
    std::cout << reset_position << ToString() << std::flush;
    reset_position = ResetPosition();
    Clear();
    delegate_->OnEvent(GetEvent());
  }

  // Restore the old terminal configuration.
  tcsetattr(STDIN_FILENO, TCSANOW, &terminal_configuration_old);
}

void ScreenInteractive::PrepareDraw() {
  auto document = delegate_->component()->Render();
  size_t dimx;
  size_t dimy;
  switch (dimension_) {
    case Dimension::Fixed:
      break;
    case Dimension::TerminalOutput:
      document->ComputeRequirement();
      dimx = Terminal::Size().dimx;
      dimy = document->requirement().min.y;
      break;
    case Dimension::Fullscreen:
      document->ComputeRequirement();
      dimx = Terminal::Size().dimx;
      dimy = Terminal::Size().dimy;
      break;
  }

  if (dimx != dimx_ || dimy != dimy_) {
    dimx_ = dimx;
    dimy_ = dimy;
    pixels_ = std::vector<std::vector<screen::Pixel>>(
        dimy, std::vector<screen::Pixel>(dimx));
  }

  Render(*this, document.get());
}

component::Delegate* ScreenInteractive::delegate() {
  return delegate_.get();
}

std::function<void()> ScreenInteractive::ExitLoopClosure() {
  return [this]() { quit_ = true; };
}

}  // namespace ftxui::component.
