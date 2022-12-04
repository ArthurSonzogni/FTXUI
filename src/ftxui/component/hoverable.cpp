#include <memory>   // for shared_ptr
#include <utility>  // for move

#include "ftxui/component/component.hpp"       // for Make, Button
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, Decorator, Element, operator|=, bgcolor, color, reflect, text, bold, border, inverted, nothing
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color
#include "ftxui/util/ref.hpp"      // for Ref, ConstStringRef

namespace ftxui {

namespace {

void Post(std::function<void()> f) {
  if (auto* screen = ScreenInteractive::Active()) {
    screen->Post(std::move(f));
    return;
  }
  f();
}

} // namespace

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param component: The wrapped component.
/// @param hover: The value to reflect whether the component is hovered or not.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto button = Button("exit", screen.ExitLoopClosure());
/// bool hover = false;
/// auto button_hover = Hoverable(button, &hover);
/// ```
Component Hoverable(Component component, bool* hover) {
  class Impl : public ComponentBase {
   public:
    Impl(Component component, bool* hover)
        : component_(component), hover_(hover) {
      Add(component_);
    }

   private:
    Element Render() override {
      return ComponentBase::Render() | reflect(box_);
    }

    bool OnEvent(Event event) override {
      if (event.is_mouse()) {
        *hover_ = box_.Contain(event.mouse().x, event.mouse().y) &&
                  CaptureMouse(event);
      }

      return ComponentBase::OnEvent(event);
    }

    Component component_;
    bool* hover_;
    Box box_;
  };

  return Make<Impl>(component, hover);
}

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param component: The wrapped component.
/// @param hover: The value to reflect whether the component is hovered or not.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto button = Button("exit", screen.ExitLoopClosure());
/// bool hover = false;
/// auto button_hover = Hoverable(button, &hover);
/// ```
Component Hoverable(Component component,
                    std::function<void()> on_enter,
                    std::function<void()> on_leave) {
  class Impl : public ComponentBase {
   public:
    Impl(Component component,
         std::function<void()> on_enter,
         std::function<void()> on_leave)
        : component_(std::move(component)),
          on_enter_(std::move(on_enter)),
          on_leave_(std::move(on_leave)) {
      Add(component_);
    }

   private:
    Element Render() override {
      return ComponentBase::Render() | reflect(box_);
    }

    bool OnEvent(Event event) override {
      if (event.is_mouse()) {
        bool hover = box_.Contain(event.mouse().x, event.mouse().y) &&
                     CaptureMouse(event);
        if (hover != hover_) {
          Post(hover ? on_enter_ : on_leave_);
        }
        hover_ = hover;
      }

      return ComponentBase::OnEvent(event);
    }

    Component component_;
    Box box_;
    bool hover_ = false;
    std::function<void()> on_enter_;
    std::function<void()> on_leave_;
  };

  return Make<Impl>(component, on_enter, on_leave);
}

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param hover: The value to reflect whether the component is hovered or not.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// bool hover = false;
/// auto button = Button("exit", screen.ExitLoopClosure());
/// button |= Hoverable(&hover);
/// ```
ComponentDecorator Hoverable(bool* hover) {
  return [hover](Component component) { return Hoverable(component, hover); };
}

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param on_enter is called when the mouse hover the component.
/// @param on_leave is called when the mouse leave the component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto button = Button("exit", screen.ExitLoopClosure());
/// int on_enter_cnt = 0;
/// int on_leave_cnt = 0;
/// button |= Hoverable(
///   [&]{ on_enter_cnt++; }, 
///   [&]{ on_leave_cnt++; }
//  );
/// ```
ComponentDecorator Hoverable(std::function<void()> on_enter,
                             std::function<void()> on_leave) {
  return [on_enter, on_leave](Component component) {
    return Hoverable(component, on_enter, on_leave);
  };
}

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param component the wrapped component.
/// @param on_change is called when the mouse enter or leave the component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto button = Button("exit", screen.ExitLoopClosure());
/// bool hovered = false;
/// auto button_hoverable = Hoverable(button,
//                                    [&](bool hover) { hovered = hover;});
/// ```
Component Hoverable(Component component, std::function<void(bool)> on_change) {
  return Hoverable(
      component,                         //
      [on_change] { on_change(true); },  //
      [on_change] { on_change(false); }  //
  );
}

/// @brief Wrap a component. Gives the ability to know if it is hovered by the
/// mouse.
/// @param on_change is called when the mouse enter or leave the component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto button = Button("exit", screen.ExitLoopClosure());
/// bool hovered = false;
/// button |= Hoverable([&](bool hover) { hovered = hover;});
/// ```
ComponentDecorator Hoverable(std::function<void(bool)> on_change) {
  return [on_change](Component component) {
    return Hoverable(component, on_change);
  };
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
