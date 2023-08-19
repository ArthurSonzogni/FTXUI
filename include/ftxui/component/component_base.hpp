// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_BASE_HPP
#define FTXUI_COMPONENT_BASE_HPP

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for CaptureMouse
#include "ftxui/dom/elements.hpp"              // for Element

namespace ftxui {

class Delegate;
class Focus;
struct Event;

namespace animation {
class Params;
}  // namespace animation

class ComponentBase;
using Component = std::shared_ptr<ComponentBase>;
using Components = std::vector<Component>;

/// @brief It implement rendering itself as ftxui::Element. It implement
/// keyboard navigation by responding to ftxui::Event.
/// @ingroup component
class ComponentBase {
 public:
  // virtual Destructor.
  virtual ~ComponentBase();

  ComponentBase() = default;

  // A component is not copiable.
  ComponentBase(const ComponentBase&) = delete;
  void operator=(const ComponentBase&) = delete;

  // Component hierarchy:
  ComponentBase* Parent() const;
  Component& ChildAt(size_t i);
  size_t ChildCount() const;
  void Add(Component children);
  void Detach();
  void DetachAllChildren();

  // Renders the component.
  virtual Element Render();

  // Handles an event.
  // By default, reduce on children with a lazy OR.
  //
  // Returns whether the event was handled or not.
  virtual bool OnEvent(Event);

  // Handle an animation step.
  virtual void OnAnimation(animation::Params& params);

  // Focus management ----------------------------------------------------------
  //
  // If this component contains children, this indicates which one is active,
  // nullptr if none is active.
  //
  // We say an element has the focus if the chain of ActiveChild() from the
  // root component contains this object.
  virtual Component ActiveChild();

  // Return true when the component contains focusable elements.
  // The non focusable Component will be skipped when navigating using the
  // keyboard.
  virtual bool Focusable() const;

  // Whether this is the active child of its parent.
  bool Active() const;
  // Whether all the ancestors are active.
  bool Focused() const;

  // Make the |child| to be the "active" one.
  virtual void SetActiveChild(ComponentBase* child);
  void SetActiveChild(Component child);

  // Configure all the ancestors to give focus to this component.
  void TakeFocus();

 protected:
  CapturedMouse CaptureMouse(const Event& event);

  Components children_;

 private:
  ComponentBase* parent_ = nullptr;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_BASE_HPP */
