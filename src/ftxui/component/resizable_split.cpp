#include <memory>   // for __shared_ptr_access
#include <utility>  // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"  // for Component, Make, Horizontal, Vertical, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/dom/elements.hpp"  // for operator|, reflect, Element, separator, size, EQUAL, xflex, yflex, hbox, vbox, HEIGHT, WIDTH
#include "ftxui/screen/box.hpp"    // for Box

namespace ftxui {
namespace {

class ResizableSplitBase : public ComponentBase {
 public:
  ResizableSplitBase(ResizableSplitOption options)
      : options_(std::move(options)) {
    Add(Container::Horizontal({
        options_->main,
        options_->back,
    }));
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
      case ResizableSplitOption::Left:
        options_->main_size() = event.mouse().x - box_.x_min;
        return true;
      case ResizableSplitOption::Right:
        options_->main_size() = box_.x_max - event.mouse().x;
        return true;
      case ResizableSplitOption::Top:
        options_->main_size() = event.mouse().y - box_.y_min;
        return true;
      case ResizableSplitOption::Bottom:
        options_->main_size() = box_.y_max - event.mouse().y;
        return true;
    }

    // NOTREACHED()
    return false;
  }

  Element Render() final {
    switch (options_->direction()) {
      case ResizableSplitOption::Left:
        return RenderLeft();
      case ResizableSplitOption::Right:
        return RenderRight();
      case ResizableSplitOption::Top:
        return RenderTop();
      case ResizableSplitOption::Bottom:
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
  };

  Element RenderRight() {
    return hbox({
               options_->back->Render() | xflex,
               options_->separator_func() | reflect(separator_box_),
               options_->main->Render() |
                   size(WIDTH, EQUAL, options_->main_size()),
           }) |
           reflect(box_);
  };

  Element RenderTop() {
    return vbox({
               options_->main->Render() |
                   size(HEIGHT, EQUAL, options_->main_size()),
               options_->separator_func() | reflect(separator_box_),
               options_->back->Render() | yflex,
           }) |
           reflect(box_);
  };

  Element RenderBottom() {
    return vbox({
               options_->back->Render() | yflex,
               options_->separator_func() | reflect(separator_box_),
               options_->main->Render() |
                   size(HEIGHT, EQUAL, options_->main_size()),
           }) |
           reflect(box_);
  };

 private:
  Ref<ResizableSplitOption> options_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

}  // namespace

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
/// auto split = ResizableSplitLeft(left, right, &left_size, separatorDouble());
/// screen.Loop(split);
/// ```
///
/// ### Output
///
/// ```bash
///           ║
///    left   ║   right
///           ║
/// ```
Component ResizableSplitLeft(Component main, Component back, int* main_size) {
  return ResizableSplit({
      .main = std::move(main),
      .back = std::move(back),
      .direction = ResizableSplitOption::Left,
      .main_size = main_size,
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
///           ║
///    left   ║   right
///           ║
/// ```
Component ResizableSplitRight(Component main, Component back, int* main_size) {
  return ResizableSplit({
      .main = std::move(main),
      .back = std::move(back),
      .direction = ResizableSplitOption::Right,
      .main_size = main_size,
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
/// ════════════
///    bottom
/// ```
Component ResizableSplitTop(Component main, Component back, int* main_size) {
  return ResizableSplit({
      .main = std::move(main),
      .back = std::move(back),
      .direction = ResizableSplitOption::Top,
      .main_size = main_size,
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
/// ════════════
///    bottom
/// ```
Component ResizableSplitBottom(Component main, Component back, int* main_size) {
  return ResizableSplit({
      .main = std::move(main),
      .back = std::move(back),
      .direction = ResizableSplitOption::Bottom,
      .main_size = main_size,
  });
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
