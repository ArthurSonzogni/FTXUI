#include "ftxui/component/component.hpp"

#include <assert.h>

#include <algorithm>

namespace ftxui {

Component::~Component() {
  Detach();
}

/// @brief Return the parent Component, or nul if any.
/// @see Attach
/// @see Detach
/// @see Parent
/// @ingroup component
Component* Component::Parent() {
  return parent_;
}

/// @brief Add a children.
/// @@param child The child to be attached.
/// @ingroup component
void Component::Add(Component* child) {
  child->Attach(this);
}

/// @brief Draw the component.
/// Build a ftxui::Element to be drawn on the ftxi::Screen representing this
/// ftxui::Component.
/// @ingroup component
Element Component::Render() {
  if (children_.size() == 1)
    return children_.front()->Render();

  return text(L"Not implemented component");
}

/// @brief Called in response to an event.
/// @param event The event.
/// @return True when the event has been handled.
/// The default implementation called OnEvent on every child until one return
/// true. If none returns true, return false.
/// @ingroup component
bool Component::OnEvent(Event event) {
  for (Component* child : children_) {
    if (child->OnEvent(event))
      return true;
  }
  return false;
}

/// @brief Return the currently Active child.
/// @return the currently Active child.
/// @ingroup component
Component* Component::ActiveChild() {
  return children_.empty() ? nullptr : children_.front();
}

/// @brief Returns if the element if the currently active child of its parent.
/// @ingroup component
bool Component::Active() {
  return !parent_ || parent_->ActiveChild() == this;
}

/// @brief Returns if the elements if focused by the user.
/// True when the Component is focused by the user. An element is Focused when
/// it is with all its ancestors the ActiveChild() of their parents.
/// @ingroup component
bool Component::Focused() {
  Component* current = this;
  for (;;) {
    Component* parent = current->parent_;
    if (!parent)
      return true;
    if (parent->ActiveChild() != current)
      return false;
    current = parent;
  }
}

/// @brief Make the |child| to be the "active" one.
/// @argument child the child to become active.
/// @ingroup component
void Component::SetActiveChild(Component*) {}

/// @brief Configure all the ancestors to give focus to this component.
/// @ingroup component
void Component::TakeFocus() {
  Component* child = this;
  Component* parent = parent_;
  while (parent) {
    parent->SetActiveChild(child);
    child = parent;
    parent = parent->parent_;
  }
}

/// @brief Detach this children from its parent.
/// @see Attach
/// @see Detach
/// @see Parent
/// @ingroup component
void Component::Detach() {
  if (!parent_)
    return;
  auto it = std::find(std::begin(parent_->children_),
                      std::end(parent_->children_), this);
  parent_->children_.erase(it);
}

/// @brief Attach this element to its parent.
/// @see Attach
/// @see Detach
/// @see Parent
/// @ingroup component
void Component::Attach(Component* parent) {
  Detach();
  parent_ = parent;
  parent_->children_.push_back(this);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
