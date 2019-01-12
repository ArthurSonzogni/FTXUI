#include "ftxui/component/container.hpp"

namespace ftxui {

// static
Container Container::Horizontal() {
  return Container(&Container::Horizontal);
}

// static
Container Container::Vertical() {
  return Container(&Container::Vertical);
}

Container::Container(Container::Handler handler) : handler_(handler) {}

bool Container::OnEvent(Event event) {
  if (!Focused())
    return false;

  if (ActiveChild()->OnEvent(event))
    return true;

  return (this->*handler_)(event);
}

Component* Container::ActiveChild() {
  return children_[selected_ % children_.size()];
}

bool Container::Vertical(Event event) {
  selected_ %= children_.size();
  // Left pressed ?
  if (event == Event::ArrowUp || event == Event::Character('k')) {
    if (selected_ != 0) {
      selected_--;
      return true;
    }
  }

  // Left pressed ?
  if (event == Event::ArrowDown || event == Event::Character('j')) {
    if (selected_ != children_.size() - 1) {
      selected_++;
      return true;
    }
  }

  return false;
}

bool Container::Horizontal(Event event) {
  selected_ %= children_.size();
  // Left pressed ?
  if (event == Event::ArrowLeft || event == Event::Character('h')) {
    if (selected_ != 0) {
      selected_--;
      return true;
    }
  }

  // Left pressed ?
  if (event == Event::ArrowRight || event == Event::Character('l')) {
    if (selected_ != children_.size() - 1) {
      selected_++;
      return true;
    }
  }

  return false;
}

}  // namespace ftxui
