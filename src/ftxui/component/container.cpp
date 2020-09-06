#include "ftxui/component/container.hpp"

#include <algorithm>

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
Container Container::Tab(int* selector) {
  Container container;
  container.event_handler_ = &Container::TabEvent;
  container.render_handler_ = &Container::TabRender;
  container.selector_ = selector;
  return container;
}

bool Container::OnEvent(Event event) {
  if (!Focused())
    return false;

  if (ActiveChild() && ActiveChild()->OnEvent(event))
    return true;

  return (this->*event_handler_)(event);
}

Component* Container::ActiveChild() {
  if (children_.size() == 0)
    return nullptr;

  int selected = selector_ ? *selector_ : selected_;
  return children_[selected % children_.size()];
}

void Container::SetActiveChild(Component* child) {
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i] == child) {
      (selector_ ? *selector_ : selected_) = i;
      return;
    }
  }
}

bool Container::VerticalEvent(Event event) {
  int old_selected = selected_;
  if (event == Event::ArrowUp || event == Event::Character('k'))
    selected_--;
  if (event == Event::ArrowDown || event == Event::Character('j'))
    selected_++;
  if (event == Event::Tab && children_.size())
    selected_ = (selected_ + 1) % children_.size();
  if (event == Event::TabReverse && children_.size())
    selected_ = (selected_ + children_.size() - 1) % children_.size();

  selected_ = std::max(0, std::min(int(children_.size()) - 1, selected_));
  return old_selected != selected_;
}

bool Container::HorizontalEvent(Event event) {
  int old_selected = selected_;
  if (event == Event::ArrowLeft || event == Event::Character('h'))
    selected_--;
  if (event == Event::ArrowRight || event == Event::Character('l'))
    selected_++;
  if (event == Event::Tab && children_.size())
    selected_ = (selected_ + 1) % children_.size();
  if (event == Event::TabReverse && children_.size())
    selected_ = (selected_ + children_.size() - 1) % children_.size();

  selected_ = std::max(0, std::min(int(children_.size()) - 1, selected_));
  return old_selected != selected_;
}

Element Container::Render() {
  return (this->*render_handler_)();
}

Element Container::VerticalRender() {
  Elements elements;
  for (auto& it : children_)
    elements.push_back(it->Render());
  if (elements.size() == 0)
    return text(L"Empty container");
  return vbox(std::move(elements));
}

Element Container::HorizontalRender() {
  Elements elements;
  for (auto& it : children_)
    elements.push_back(it->Render());
  if (elements.size() == 0)
    return text(L"Empty container");
  return hbox(std::move(elements));
}

Element Container::TabRender() {
  Component* active_child = ActiveChild();
  if (active_child)
    return active_child->Render();
  return text(L"Empty container");
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
