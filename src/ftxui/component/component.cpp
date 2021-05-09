#include <algorithm>           // for find_if
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <iterator>            // for begin, end
#include <utility>             // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse, CapturedMouseInterface
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                  // for text, Element

namespace ftxui {

namespace {
class CaptureMouseImpl : public CapturedMouseInterface {
 public:
  ~CaptureMouseImpl() override {}
};
}  // namespace

ComponentBase::~ComponentBase() {
  Detach();
}

/// @brief Return the parent ComponentBase, or nul if any.
/// @see Attach
/// @see Detach
/// @see Parent
/// @ingroup component
ComponentBase* ComponentBase::Parent() {
  return parent_;
}

/// @brief Add a children.
/// @@param child The child to be attached.
/// @ingroup component
void ComponentBase::Add(Component child) {
  child->Detach();
  child->parent_ = this;
  children_.push_back(std::move(child));
}

/// @brief Draw the component.
/// Build a ftxui::Element to be drawn on the ftxi::Screen representing this
/// ftxui::ComponentBase.
/// @ingroup component
Element ComponentBase::Render() {
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
bool ComponentBase::OnEvent(Event event) {
  for (Component& child : children_) {
    if (child->OnEvent(event))
      return true;
  }
  return false;
}

/// @brief Return the currently Active child.
/// @return the currently Active child.
/// @ingroup component
Component ComponentBase::ActiveChild() {
  return children_.empty() ? nullptr : children_.front();
}

/// @brief Returns if the element if the currently active child of its parent.
/// @ingroup component
bool ComponentBase::Active() {
  return !parent_ || parent_->ActiveChild().get() == this;
}

/// @brief Returns if the elements if focused by the user.
/// True when the ComponentBase is focused by the user. An element is Focused
/// when it is with all its ancestors the ActiveChild() of their parents.
/// @ingroup component
bool ComponentBase::Focused() {
  ComponentBase* current = this;
  for (;;) {
    ComponentBase* parent = current->parent_;
    if (!parent)
      return true;
    if (parent->ActiveChild().get() != current)
      return false;
    current = parent;
  }
}

/// @brief Make the |child| to be the "active" one.
/// @argument child the child to become active.
/// @ingroup component
void ComponentBase::SetActiveChild(ComponentBase*) {}

/// @brief Make the |child| to be the "active" one.
/// @argument child the child to become active.
/// @ingroup component
void ComponentBase::SetActiveChild(Component child) {
  SetActiveChild(child.get());
}

/// @brief Configure all the ancestors to give focus to this component.
/// @ingroup component
void ComponentBase::TakeFocus() {
  ComponentBase* child = this;
  ComponentBase* parent = parent_;
  while (parent) {
    parent->SetActiveChild(child);
    child = parent;
    parent = parent->parent_;
  }
}

/// @brief Take the CapturedMouse if available. There is only one component of
/// them. It represents a component taking priority over others.
/// @argument event
/// @ingroup component
CapturedMouse ComponentBase::CaptureMouse(const Event& event) {
  if (!event.screen_)
    return std::make_unique<CaptureMouseImpl>();
  return event.screen_->CaptureMouse();
}

/// @brief Detach this children from its parent.
/// @see Attach
/// @see Detach
/// @see Parent
/// @ingroup component
void ComponentBase::Detach() {
  if (!parent_)
    return;
  auto it = std::find_if(std::begin(parent_->children_),  //
                         std::end(parent_->children_),    //
                         [this](const Component& that) {  //
                           return this == that.get();
                         });
  parent_->children_.erase(it);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
