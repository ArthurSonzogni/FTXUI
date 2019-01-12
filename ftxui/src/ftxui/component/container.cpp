#include "ftxui/component/container.hpp"

namespace ftxui {

// static
Container Container::Horizontal() {
  Container container;
  container.event_handler_ = &Container::HorizontalEvent;
  container.render_handler_ = &Container::HorizontalRender;
  return container;
}

// static
Container Container::Vertical() {
  Container container;
  container.event_handler_ = &Container::VerticalEvent;
  container.render_handler_ = &Container::VerticalRender;
  return container;
}

// static
Container Container::Tab(size_t* selector) {
  Container container;
  container.event_handler_ = &Container::TabEvent;
  container.render_handler_ = &Container::TabRender;
  container.selector_ = selector;
  return container;
}

bool Container::OnEvent(Event event) {
  if (!Focused())
    return false;

  if (ActiveChild()->OnEvent(event))
    return true;

  return (this->*event_handler_)(event);
}

Component* Container::ActiveChild() {
  return children_[*selector_ % children_.size()];
}

bool Container::VerticalEvent(Event event) {
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

bool Container::HorizontalEvent(Event event) {
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

Element Container::Render() {
  return (this->*render_handler_)();
}

Element Container::VerticalRender() {
  Elements elements;
  for(auto& it : children_)
    elements.push_back(it->Render());
  return vbox(std::move(elements));
}

Element Container::HorizontalRender() {
  Elements elements;
  for(auto& it : children_)
    elements.push_back(it->Render());
  return hbox(std::move(elements));
}

Element Container::TabRender() {
  return ActiveChild()->Render();
}

}  // namespace ftxui
