#ifndef FTXUI_COMPONENT_CONTAINER_HPP
#define FTXUI_COMPONENT_CONTAINER_HPP

#include "ftxui/component/component.hpp"       // for Component, Components
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/elements.hpp"              // for Element

namespace ftxui {

/// @brief A component where focus and events are automatically handled for you.
class ContainerBase : public ComponentBase {
 public:
  static Component Vertical();
  static Component Vertical(Components children);

  static Component Horizontal();
  static Component Horizontal(Components children);

  static Component Tab(int* selector);
  static Component Tab(Components children, int* selector);

  ~ContainerBase() override = default;

  // Component override.
  bool OnEvent(Event event) override;
  Element Render() override;
  Component ActiveChild() override;
  virtual void SetActiveChild(ComponentBase*) override;

 protected:
  // Handlers
  using EventHandler = bool (ContainerBase::*)(Event);
  bool VerticalEvent(Event event);
  bool HorizontalEvent(Event event);
  bool TabEvent(Event) { return false; }
  EventHandler event_handler_;

  using RenderHandler = Element (ContainerBase::*)();
  Element VerticalRender();
  Element HorizontalRender();
  Element TabRender();
  RenderHandler render_handler_;

  int selected_ = 0;
  int* selector_ = nullptr;

 private:
  bool OnMouseEvent(Event event);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CONTAINER_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
