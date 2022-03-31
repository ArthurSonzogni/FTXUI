#include <algorithm>  // for max, min
#include <cstddef>    // for size_t
#include <memory>  // for make_shared, __shared_ptr_access, allocator, shared_ptr, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/component/component.hpp"  // for Horizontal, Vertical, Tab
#include "ftxui/component/component_base.hpp"  // for Components, Component, ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::Tab, Event::TabReverse, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp, Event::End, Event::Home, Event::PageDown, Event::PageUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::WheelDown, Mouse::WheelUp
#include "ftxui/dom/elements.hpp"  // for text, Elements, operator|, reflect, Element, hbox, vbox
#include "ftxui/screen/box.hpp"  // for Box

namespace ftxui {

class ContainerBase : public ComponentBase {
 public:
  ContainerBase(Components children, int* selector)
      : selector_(selector ? selector : &selected_) {
    for (Component& child : children) {
      Add(std::move(child));
    }
  }

  // Component override.
  bool OnEvent(Event event) override {
    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    if (!Focused()) {
      return false;
    }

    if (ActiveChild() && ActiveChild()->OnEvent(event)) {
      return true;
    }

    return EventHandler(event);
  }

  Component ActiveChild() override {
    if (children_.empty()) {
      return nullptr;
    }

    return children_[*selector_ % children_.size()];
  }

  void SetActiveChild(ComponentBase* child) override {
    for (size_t i = 0; i < children_.size(); ++i) {
      if (children_[i].get() == child) {
        *selector_ = (int)i;
        return;
      }
    }
  }

 protected:
  // Handlers
  virtual bool EventHandler(Event /*unused*/) { return false; }  // NOLINT

  virtual bool OnMouseEvent(Event event) {
    return ComponentBase::OnEvent(std::move(event));
  }

  int selected_ = 0;
  int* selector_ = nullptr;

  void MoveSelector(int dir) {
    for (int i = *selector_ + dir; i >= 0 && i < (int)children_.size();
         i += dir) {
      if (children_[i]->Focusable()) {
        *selector_ = i;
        return;
      }
    }
  }

  void MoveSelectorWrap(int dir) {
    if (children_.empty()) {
      return;
    }
    for (size_t offset = 1; offset < children_.size(); ++offset) {
      size_t i = ((size_t(*selector_ + offset * dir + children_.size())) %
                  children_.size());
      if (children_[i]->Focusable()) {
        *selector_ = (int)i;
        return;
      }
    }
  }
};

class VerticalContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Elements elements;
    for (auto& it : children_) {
      elements.push_back(it->Render());
    }
    if (elements.empty()) {
      return text("Empty container") | reflect(box_);
    }
    return vbox(std::move(elements)) | reflect(box_);
  }

  bool EventHandler(Event event) override {
    int old_selected = *selector_;
    if (event == Event::ArrowUp || event == Event::Character('k')) {
      MoveSelector(-1);
    }
    if (event == Event::ArrowDown || event == Event::Character('j')) {
      MoveSelector(+1);
    }
    if (event == Event::PageUp) {
      for (int i = 0; i < box_.y_max - box_.y_min; ++i) {
        MoveSelector(-1);
      }
    }
    if (event == Event::PageDown) {
      for (int i = 0; i < box_.y_max - box_.y_min; ++i) {
        MoveSelector(1);
      }
    }
    if (event == Event::Home) {
      for (size_t i = 0; i < children_.size(); ++i) {
        MoveSelector(-1);
      }
    }
    if (event == Event::End) {
      for (size_t i = 0; i < children_.size(); ++i) {
        MoveSelector(1);
      }
    }
    if (event == Event::Tab) {
      MoveSelectorWrap(+1);
    }
    if (event == Event::TabReverse) {
      MoveSelectorWrap(-1);
    }

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }

  bool OnMouseEvent(Event event) override {
    if (ContainerBase::OnMouseEvent(event)) {
      return true;
    }

    if (event.mouse().button != Mouse::WheelUp &&
        event.mouse().button != Mouse::WheelDown) {
      return false;
    }

    if (!box_.Contain(event.mouse().x, event.mouse().y)) {
      return false;
    }

    if (event.mouse().button == Mouse::WheelUp) {
      MoveSelector(-1);
    }
    if (event.mouse().button == Mouse::WheelDown) {
      MoveSelector(+1);
    }
    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));

    return true;
  }

  Box box_;
};

class HorizontalContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Elements elements;
    for (auto& it : children_) {
      elements.push_back(it->Render());
    }
    if (elements.empty()) {
      return text("Empty container");
    }
    return hbox(std::move(elements));
  }

  bool EventHandler(Event event) override {
    int old_selected = *selector_;
    if (event == Event::ArrowLeft || event == Event::Character('h')) {
      MoveSelector(-1);
    }
    if (event == Event::ArrowRight || event == Event::Character('l')) {
      MoveSelector(+1);
    }
    if (event == Event::Tab) {
      MoveSelectorWrap(+1);
    }
    if (event == Event::TabReverse) {
      MoveSelectorWrap(-1);
    }

    *selector_ = std::max(0, std::min(int(children_.size()) - 1, *selector_));
    return old_selected != *selector_;
  }
};

class TabContainer : public ContainerBase {
 public:
  using ContainerBase::ContainerBase;

  Element Render() override {
    Component active_child = ActiveChild();
    if (active_child) {
      return active_child->Render();
    }
    return text("Empty container");
  }

  bool Focusable() const override {
    if (children_.empty()) {
      return false;
    }
    return children_[*selector_ % children_.size()]->Focusable();
  }

  bool OnMouseEvent(Event event) override {
    return ActiveChild()->OnEvent(event);
  }
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
  return Vertical(std::move(children), nullptr);
}

/// @brief A list of components, drawn one by one vertically and navigated
/// vertically using up/down arrow key or 'j'/'k' keys.
/// This is useful for implementing a Menu for instance.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
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
  return std::make_shared<VerticalContainer>(std::move(children), selector);
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
  return Horizontal(std::move(children), nullptr);
}

/// @brief A list of components, drawn one by one horizontally and navigated
/// horizontally using left/right arrow key or 'h'/'l' keys.
/// @param children the list of components.
/// @param selector A reference to the index of the selected children.
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
  return std::make_shared<HorizontalContainer>(std::move(children), selector);
}

/// @brief A list of components, where only one is drawn and interacted with at
/// a time. The |selector| gives the index of the selected component. This is
/// useful to implement tabs.
/// @param children The list of components.
/// @param selector The index of the drawn children.
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
  return std::make_shared<TabContainer>(std::move(children), selector);
}

}  // namespace Container

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
