#include <stddef.h>   // for size_t
#include <algorithm>  // for max, min
#include <memory>  // for __shared_ptr_access, shared_ptr, make_shared, allocator, __shared_ptr_access<>::element_type, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/container.hpp"

namespace ftxui {

namespace Container {

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Vertical({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component Vertical(Components children) {
  return ContainerBase::Vertical(std::move(children));
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component Horizontal(Components children) {
  return ContainerBase::Horizontal(std::move(children));
}

/// @brief A list of components, where only one is drawn and interacted with at
/// a time. The |selector| gives the index of the selected component. This is
/// useful to implement tabs.
/// @param selector The index of the drawn children.
/// @param children the list of components.
/// @ingroup component
/// @see ContainerBase
///
/// ### Example
///
/// ```cpp
/// int tab_drawn = 0;
/// auto container = Container::Tab(&tab_drawn, {
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// });
/// ```
Component Tab(int* selector, Components children) {
  return ContainerBase::Tab(selector, std::move(children));
}

}  // namespace Container

// static
Component ContainerBase::Vertical() {
  return Vertical({});
}

// static
Component ContainerBase::Vertical(Components children) {
  auto container = std::make_shared<ContainerBase>();
  container->event_handler_ = &ContainerBase::VerticalEvent;
  container->render_handler_ = &ContainerBase::VerticalRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

// static
Component ContainerBase::Horizontal() {
  return Horizontal({});
}

// static
Component ContainerBase::Horizontal(Components children) {
  auto container = std::make_shared<ContainerBase>();
  container->event_handler_ = &ContainerBase::HorizontalEvent;
  container->render_handler_ = &ContainerBase::HorizontalRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

// static
Component ContainerBase::Tab(int* selector) {
  return Tab(selector, {});
}

// static
Component ContainerBase::Tab(int* selector, Components children) {
  auto container = std::make_shared<ContainerBase>();
  container->selector_ = selector;
  container->event_handler_ = &ContainerBase::TabEvent;
  container->render_handler_ = &ContainerBase::TabRender;
  for (Component& child : children)
    container->Add(std::move(child));
  return container;
}

bool ContainerBase::OnEvent(Event event) {
  if (event.is_mouse())
    return OnMouseEvent(event);

  if (!Focused())
    return false;

  if (ActiveChild() && ActiveChild()->OnEvent(event))
    return true;

  return (this->*event_handler_)(event);
}

Component ContainerBase::ActiveChild() {
  if (children_.size() == 0)
    return nullptr;

  int selected = selector_ ? *selector_ : selected_;
  return children_[selected % children_.size()];
}

void ContainerBase::SetActiveChild(ComponentBase* child) {
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i].get() == child) {
      (selector_ ? *selector_ : selected_) = i;
      return;
    }
  }
}

bool ContainerBase::VerticalEvent(Event event) {
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

bool ContainerBase::HorizontalEvent(Event event) {
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

Element ContainerBase::Render() {
  return (this->*render_handler_)();
}

Element ContainerBase::VerticalRender() {
  Elements elements;
  for (auto& it : children_)
    elements.push_back(it->Render());
  if (elements.size() == 0)
    return text(L"Empty container");
  return vbox(std::move(elements));
}

Element ContainerBase::HorizontalRender() {
  Elements elements;
  for (auto& it : children_)
    elements.push_back(it->Render());
  if (elements.size() == 0)
    return text(L"Empty container");
  return hbox(std::move(elements));
}

Element ContainerBase::TabRender() {
  Component active_child = ActiveChild();
  if (active_child)
    return active_child->Render();
  return text(L"Empty container");
}

bool ContainerBase::OnMouseEvent(Event event) {
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
