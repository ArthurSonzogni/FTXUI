#ifndef FTXUI_COMPONENT_COMPONENT_HPP
#define FTXUI_COMPONENT_COMPONENT_HPP

#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

class Delegate;
class Focus;

/// @brief It implement rendering itself as ftxui::Element. It implement
/// keyboard navigation by responding to ftxui::Event.
/// @ingroup component
class Component {
 public:
  // Constructor/Destructor.
  Component() = default;
  virtual ~Component();

  // Component hierarchy.
  Component* Parent();
  void Add(Component* children);

  // Renders the component.
  virtual Element Render();

  // Handles an event.
  // By default, reduce on children with a lazy OR.
  //
  // Returns whether the event was handled or not.
  virtual bool OnEvent(Event);

  // Focus management ----------------------------------------------------------
  //
  // If this component contains children, this indicates which one is active,
  // nullptr if none is active.
  //
  // We say an element has the focus if the chain of ActiveChild() from the
  // root component contains this object.
  virtual Component* ActiveChild();

  // Whether this is the active child of its parent.
  bool Active();
  // Whether all the ancestors are active.
  bool Focused();

  // Make the |child| to be the "active" one.
  virtual void SetActiveChild(Component* child);

  // Configure all the ancestors to give focus to this component.
  void TakeFocus();

 private:
  Component* parent_ = nullptr;
  void Detach();
  void Attach(Component* parent);

 protected:
  std::vector<Component*> children_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
