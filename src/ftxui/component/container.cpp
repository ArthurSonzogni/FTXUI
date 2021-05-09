#include <stddef.h>            // for size_t
#include <algorithm>           // for max, min
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <memory>  // for __shared_ptr_access, shared_ptr, make_shared, __shared_ptr_access<>::element_type, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, allocator

#include "ftxui/component/container.hpp"

namespace ftxui {

Component ContainerVertical(Components children) {
  return Container::Vertical(std::move(children));
}

Component ContainerHorizontal(Components children) {
  return Container::Horizontal(std::move(children));
}

Component ContainerTab(int* selector, Components children) {
  return Container::Tab(selector, std::move(children));
}

// static
Component Container::Vertical() {
  return Vertical({});
}

// static
Component Container::Vertical(Components children) {
  auto container = std::make_shared<Container>();
  container->event_handler_ = &Container::VerticalEvent;
  container->render_handler_ = &Container::VerticalRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

// static
Component Container::Horizontal() {
  return Horizontal({});
}

// static
Component Container::Horizontal(Components children) {
  auto container = std::make_shared<Container>();
  container->event_handler_ = &Container::HorizontalEvent;
  container->render_handler_ = &Container::HorizontalRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

// static
Component Container::Tab(int* selector) {
  return Tab(selector, {});
}

// static
Component Container::Tab(int* selector, Components children) {
  auto container = std::make_shared<Container>();
  container->selector_ = selector;
  container->event_handler_ = &Container::TabEvent;
  container->render_handler_ = &Container::TabRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

bool Container::OnEvent(Event event) {
  if (event.is_mouse())
    return OnMouseEvent(event);

  if (!Focused())
    return false;

  if (ActiveChild() && ActiveChild()->OnEvent(event))
    return true;

  return (this->*event_handler_)(event);
}

Component Container::ActiveChild() {
  if (children_.size() == 0)
    return nullptr;

  int selected = selector_ ? *selector_ : selected_;
  return children_[selected % children_.size()];
}

void Container::SetActiveChild(ComponentBase* child) {
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i].get() == child) {
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
  Component active_child = ActiveChild();
  if (active_child)
    return active_child->Render();
  return text(L"Empty container");
}

bool Container::OnMouseEvent(Event event) {
  if (selector_)
    return ActiveChild()->OnEvent(event);

  for (Component& child : children_) {
    if (child->OnEvent(event))
      return true;
  }
  return false;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
