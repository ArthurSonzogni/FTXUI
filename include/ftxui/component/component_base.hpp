// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_BASE_HPP
#define FTXUI_COMPONENT_BASE_HPP

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for CaptureMouse
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/util/export.hpp"

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
class FTXUI_EXPORT(COMPONENT) ComponentBase {
 public:
  explicit ComponentBase(Components children);
  virtual ~ComponentBase();
  ComponentBase();

  // A component is not copyable/movable.
  ComponentBase(const ComponentBase&) = delete;
  ComponentBase(ComponentBase&&) = delete;
  ComponentBase& operator=(const ComponentBase&) = delete;
  ComponentBase& operator=(ComponentBase&&) = delete;

  // Component hierarchy:

  /// Return the parent component, or nullptr if this is the root.
  ComponentBase* Parent() const;
  /// Return the child component at index `i`.
  Component& ChildAt(size_t i);
  /// Return the number of direct children.
  size_t ChildCount() const;
  /// Return this component's index within its parent, or -1 if detached.
  int Index() const;
  /// Add `children` as a child of this component.
  void Add(Component children);
  /// Remove this component from its parent.
  void Detach();
  /// Remove all child components.
  void DetachAllChildren();

  /// Render the component into a DOM Element.
  Element Render();

  /// Override to customize how this component renders itself.
  virtual Element OnRender();

  /// Handle an input event; return true if the event was consumed.
  virtual bool OnEvent(Event);

  /// Handle an animation step.
  virtual void OnAnimation(animation::Params& params);

  // Focus management ----------------------------------------------------------

  /// Return the currently active child component, or nullptr if none.
  virtual Component ActiveChild();

  /// Return true if this component (or a descendant) can receive focus.
  virtual bool Focusable() const;

  /// Return true if this component is the active child of its parent.
  bool Active() const;
  /// Return true if this component and all of its ancestors are active.
  bool Focused() const;

  /// Mark `child` as the active child of this component.
  virtual void SetActiveChild(ComponentBase* child);
  /// Mark `child` as the active child of this component.
  void SetActiveChild(Component child);

  /// Configure all ancestors so that this component receives focus.
  void TakeFocus();

  // ABI Reserve:
  virtual void Reserved1();
  virtual void Reserved2();
  virtual void Reserved3();
  virtual void Reserved4();
  virtual void Reserved5();
  virtual void Reserved6();
  virtual void Reserved7();
  virtual void Reserved8();

 protected:
  CapturedMouse CaptureMouse(const Event& event);

  Components& children();
  const Components& children() const;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_BASE_HPP */
