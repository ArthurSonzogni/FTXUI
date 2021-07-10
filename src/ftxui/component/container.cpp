#include <stddef.h>  // for size_t

#include <algorithm>  // for max, min
#include <memory>  // for __shared_ptr_access, shared_ptr, make_shared, allocator, __shared_ptr_access<>::element_type, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"

namespace ftxui {

/// @brief A component where focus and events are automatically handled for you.
class ContainerBase : public ComponentBase {
 public:
  static Component Vertical() { return Vertical({}); }
  static Component Vertical(Components children) {
    return Vertical(std::move(children), /*selector=*/nullptr);
  }
  static Component Vertical(Components children, int* selector) {
    auto container = std::make_shared<ContainerBase>();
    container->event_handler_ = &ContainerBase::VerticalEvent;
    container->render_handler_ = &ContainerBase::VerticalRender;
    container->selector_ = selector ? selector : &container->selected_;
    for (Component& child : children)
      container->Add(std::move(child));
    return container;
  }

  static Component Horizontal() { return Horizontal({}); }
  static Component Horizontal(Components children) {
    return Horizontal(std::move(children), /*selector=*/nullptr);
  }
  static Component Horizontal(Components children, int* selector) {
    auto container = std::make_shared<ContainerBase>();
    container->event_handler_ = &ContainerBase::HorizontalEvent;
    container->render_handler_ = &ContainerBase::HorizontalRender;
    container->selector_ = selector ? selector : &container->selected_;
    for (Component& child : children)
      container->Add(std::move(child));
    return container;
  }

  static Component Tab(int* selector) { return Tab({}, selector); }
  static Component Tab(Components children, int* selector) {
    auto container = std::make_shared<ContainerBase>();
    container->selector_ = selector ? selector : &container->selected_;
    container->event_handler_ = &ContainerBase::TabEvent;
    container->render_handler_ = &ContainerBase::TabRender;
    container->is_tab_ = true;
    for (Component& child : children)
      container->Add(std::move(child));
    return container;
  }

  ~ContainerBase() override = default;

  // Component override.
  bool OnEvent(Event event) override {
    if (event.is_mouse())
      return OnMouseEvent(event);

    if (!Focused())
      return false;

    if (ActiveChild() && ActiveChild()->OnEvent(event))
      return true;

    return (this->*event_handler_)(event);
  }

  Component ActiveChild() override {
    if (children_.size() == 0)
      return nullptr;

    return children_[*selector_ % children_.size()];
  }

  void SetActiveChild(ComponentBase* child) override {
    for (size_t i = 0; i < children_.size(); ++i) {
      if (children_[i].get() == child) {
        *selector_ = i;
        return;
      }
    }
  }

 private:
  // Handlers

  bool VerticalEvent(Event event) {
    int old_selected = *selector_;
    if (event == Event::ArrowUp || event == Event::Character('k'))
      (*selector_)--;
    if (event == Event::ArrowDown || event == Event::Character('j'))
      (*selector_)++;
    if (event == Event::Tab && children_.size())
      *selector_ = (*selector_ + 1) % children_.size();
    if (event == Event::TabReverse && children_.size())
      *selector_ = (*selector_ + children_.size() - 1) % children_.size();

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }

  bool HorizontalEvent(Event event) {
    int old_selected = *selector_;
    if (event == Event::ArrowLeft || event == Event::Character('h'))
      (*selector_)--;
    if (event == Event::ArrowRight || event == Event::Character('l'))
      (*selector_)++;
    if (event == Event::Tab && children_.size())
      *selector_ = (*selector_ + 1) % children_.size();
    if (event == Event::TabReverse && children_.size())
      *selector_ = (*selector_ + children_.size() - 1) % children_.size();

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }

  bool TabEvent(Event) { return false; }

  bool OnMouseEvent(Event event) {
    if (is_tab_)
      return ActiveChild()->OnEvent(event);

    for (Component& child : children_) {
      if (child->OnEvent(event))
        return true;
    }
    return false;
  }

  using EventHandler = bool (ContainerBase::*)(Event);
  using RenderHandler = Element (ContainerBase::*)();

  Element Render() override { return (this->*render_handler_)(); }

  Element VerticalRender() {
    Elements elements;
    for (auto& it : children_)
      elements.push_back(it->Render());
    if (elements.size() == 0)
      return text(L"Empty container");
    return vbox(std::move(elements));
  }

  Element HorizontalRender() {
    Elements elements;
    for (auto& it : children_)
      elements.push_back(it->Render());
    if (elements.size() == 0)
      return text(L"Empty container");
    return hbox(std::move(elements));
  }

  Element TabRender() {
    Component active_child = ActiveChild();
    if (active_child)
      return active_child->Render();
    return text(L"Empty container");
  }

  EventHandler event_handler_;
  RenderHandler render_handler_;
  int selected_ = 0;
  int* selector_ = nullptr;
  bool is_tab_ = false;
};

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

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// This is useful for implementing a Menu for instance.
/// @param children the list of components.
/// @param selector An integer storing the selected children.
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
Component Vertical(Components children, int* selector) {
  return ContainerBase::Vertical(std::move(children), selector);
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
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, &selected_children);
/// ```
Component Horizontal(Components children) {
  return ContainerBase::Horizontal(std::move(children));
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
/// int selected_children = 2;
/// auto container = Container::Horizontal({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, selected_children);
/// ```
Component Horizontal(Components children, int* selector) {
  return ContainerBase::Horizontal(std::move(children), selector);
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
/// auto container = Container::Tab({
///   children_1,
///   children_2,
///   children_3,
///   children_4,
/// }, &tab_drawn);
/// ```
Component Tab(Components children, int* selector) {
  return ContainerBase::Tab(std::move(children), selector);
}

}  // namespace Container

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
