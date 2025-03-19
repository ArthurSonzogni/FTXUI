// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/component_options.hpp>  // for ResizableSplitOption
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <ftxui/util/ref.hpp>       // for Ref
#include <functional>               // for function
#include <utility>                  // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"  // for Horizontal, Make, ResizableSplit, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/dom/elements.hpp"  // for operator|, reflect, Element, size, EQUAL, xflex, yflex, hbox, vbox, HEIGHT, WIDTH, text
#include "ftxui/screen/box.hpp"    // for Box

namespace ftxui {
namespace {

class ResizableSplitBase : public ComponentBase {
 public:
  explicit ResizableSplitBase(ResizableSplitOption options)
      : options_(std::move(options)) {
    switch (options_->direction()) {
      case Direction::Left:
        Add(Container::Horizontal({
            options_->main,
            options_->back,
        }));
        break;
      case Direction::Right:
        Add(Container::Horizontal({
            options_->back,
            options_->main,
        }));
        break;
      case Direction::Up:
        Add(Container::Vertical({
            options_->main,
            options_->back,
        }));
        break;
      case Direction::Down:
        Add(Container::Vertical({
            options_->back,
            options_->main,
        }));
        break;
    }
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse()) {
      return OnMouseEvent(std::move(event));
    }
    return ComponentBase::OnEvent(std::move(event));
  }

  bool OnMouseEvent(Event event) {
    if (captured_mouse_ && event.mouse().motion == Mouse::Released) {
      captured_mouse_.reset();
      return true;
    }

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed &&
        separator_box_.Contain(event.mouse().x, event.mouse().y) &&
        !captured_mouse_) {
      captured_mouse_ = CaptureMouse(event);
      return true;
    }

    if (!captured_mouse_) {
      return ComponentBase::OnEvent(event);
    }

    switch (options_->direction()) {
      case Direction::Left:
        options_->main_size() = event.mouse().x - box_.x_min;
        return true;
      case Direction::Right:
        options_->main_size() = box_.x_max - event.mouse().x;
        return true;
      case Direction::Up:
        options_->main_size() = event.mouse().y - box_.y_min;
        return true;
      case Direction::Down:
        options_->main_size() = box_.y_max - event.mouse().y;
        return true;
    }

    // NOTREACHED()
    return false;
  }

  Element OnRender() final {
    switch (options_->direction()) {
      case Direction::Left:
        return RenderLeft();
      case Direction::Right:
        return RenderRight();
      case Direction::Up:
        return RenderTop();
      case Direction::Down:
        return RenderBottom();
    }
    // NOTREACHED()
    return text("unreacheable");
  }

  Element RenderLeft() {
    return hbox({
               options_->main->Render() |
                   size(WIDTH, EQUAL, options_->main_size()),
               options_->separator_func() | reflect(separator_box_),
               options_->back->Render() | xflex,
           }) |
           reflect(box_);
  }

  Element RenderRight() {
    return hbox({
               options_->back->Render() | xflex,
               options_->separator_func() | reflect(separator_box_),
               options_->main->Render() |
                   size(WIDTH, EQUAL, options_->main_size()),
           }) |
           reflect(box_);
  }

  Element RenderTop() {
    return vbox({
               options_->main->Render() |
                   size(HEIGHT, EQUAL, options_->main_size()),
               options_->separator_func() | reflect(separator_box_),
               options_->back->Render() | yflex,
           }) |
           reflect(box_);
  }

  Element RenderBottom() {
    return vbox({
               options_->back->Render() | yflex,
               options_->separator_func() | reflect(separator_box_),
               options_->main->Render() |
                   size(HEIGHT, EQUAL, options_->main_size()),
           }) |
           reflect(box_);
  }

 private:
  Ref<ResizableSplitOption> options_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

}  // namespace

/// @brief A split in between two components.
/// @param options all the parameters.
///
/// ### Example
///
/// ```cpp
/// auto left = Renderer([] { return text("Left") | center;});
/// auto right = Renderer([] { return text("right") | center;});
/// int left_size = 10;
/// auto component = ResizableSplit({
///   .main = left,
///   .back = right,
///   .direction = Direction::Left,
///   .main_size = &left_size,
///   .separator_func = [] { return separatorDouble(); },
/// });
/// ```
///
/// ### Output
///
/// ```bash
///           ║
///    left   ║   right
///           ║
/// ```
Component ResizableSplit(ResizableSplitOption options) {
  return Make<ResizableSplitBase>(std::move(options));
}

/// @brief An horizontal split in between two components, configurable using the
/// mouse.
/// @param main The main component of size |main_size|, on the left.
/// @param back The back component taking the remaining size, on the right.
/// @param main_size The size of the |main| component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::Fullscreen();
/// int left_size = 10;
/// auto left = Renderer([] { return text("Left") | center;});
/// auto right = Renderer([] { return text("right") | center;});
/// auto split = ResizableSplitLeft(left, right, &left_size);
/// screen.Loop(split);
/// ```
///
/// ### Output
///
/// ```bash
///           │
///    left   │   right
///           │
/// ```
Component ResizableSplitLeft(Component main, Component back, int* main_size) {
  return ResizableSplit({
      std::move(main),
      std::move(back),
      Direction::Left,
      main_size,
  });
}

/// @brief An horizontal split in between two components, configurable using the
/// mouse.
/// @param main The main component of size |main_size|, on the right.
/// @param back The back component taking the remaining size, on the left.
/// @param main_size The size of the |main| component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::Fullscreen();
/// int right_size = 10;
/// auto left = Renderer([] { return text("Left") | center;});
/// auto right = Renderer([] { return text("right") | center;});
/// auto split = ResizableSplitRight(right, left, &right_size)
/// screen.Loop(split);
/// ```
///
/// ### Output
///
/// ```bash
///           │
///    left   │   right
///           │
/// ```
Component ResizableSplitRight(Component main, Component back, int* main_size) {
  return ResizableSplit({
      std::move(main),
      std::move(back),
      Direction::Right,
      main_size,
  });
}

/// @brief An vertical split in between two components, configurable using the
/// mouse.
/// @param main The main component of size |main_size|, on the top.
/// @param back The back component taking the remaining size, on the bottom.
/// @param main_size The size of the |main| component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::Fullscreen();
/// int top_size = 1;
/// auto top = Renderer([] { return text("Top") | center;});
/// auto bottom = Renderer([] { return text("Bottom") | center;});
/// auto split = ResizableSplitTop(top, bottom, &top_size)
/// screen.Loop(split);
/// ```
///
/// ### Output
///
/// ```bash
///    top
/// ────────────
///    bottom
/// ```
Component ResizableSplitTop(Component main, Component back, int* main_size) {
  return ResizableSplit({
      std::move(main),
      std::move(back),
      Direction::Up,
      main_size,
  });
}

/// @brief An vertical split in between two components, configurable using the
/// mouse.
/// @param main The main component of size |main_size|, on the bottom.
/// @param back The back component taking the remaining size, on the top.
/// @param main_size The size of the |main| component.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::Fullscreen();
/// int bottom_size = 1;
/// auto top = Renderer([] { return text("Top") | center;});
/// auto bottom = Renderer([] { return text("Bottom") | center;});
/// auto split = ResizableSplit::Bottom(bottom, top, &bottom_size)
/// screen.Loop(split);
/// ```
///
/// ### Output
///
/// ```bash
///    top
/// ────────────
///    bottom
/// ```
Component ResizableSplitBottom(Component main, Component back, int* main_size) {
  return ResizableSplit({
      std::move(main),
      std::move(back),
      Direction::Down,
      main_size,
  });
}

}  // namespace ftxui
