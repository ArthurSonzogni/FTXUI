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

class ResizableSplitLeftBase : public ComponentBase {
 public:
  ResizableSplitLeftBase(Component main, Component child, int* main_size)
      : main_(std::move(main)),
        child_(std::move(child)),
        main_size_(main_size) {
    Add(Container::Horizontal({
        main_,
        child_,
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

    if (captured_mouse_) {
      *main_size_ = event.mouse().x - box_.x_min;
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  Element Render() final {
    return hbox({
               main_->Render() | size(WIDTH, EQUAL, *main_size_),
               separator() | reflect(separator_box_),
               child_->Render() | xflex,
           }) |
           reflect(box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

class ResizableSplitRightBase : public ComponentBase {
 public:
  ResizableSplitRightBase(Component main, Component child, int* main_size)
      : main_(std::move(main)),
        child_(std::move(child)),
        main_size_(main_size) {
    Add(Container::Horizontal({
        child_,
        main_,
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

    if (captured_mouse_) {
      *main_size_ = box_.x_max - event.mouse().x;
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  Element Render() final {
    return hbox({
               child_->Render() | xflex,
               separator() | reflect(separator_box_),
               main_->Render() | size(WIDTH, EQUAL, *main_size_),
           }) |
           reflect(box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

class ResizableSplitTopBase : public ComponentBase {
 public:
  ResizableSplitTopBase(Component main, Component child, int* main_size)
      : main_(std::move(main)),
        child_(std::move(child)),
        main_size_(main_size) {
    Add(Container::Vertical({
        main_,
        child_,
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

    if (captured_mouse_) {
      *main_size_ = event.mouse().y - box_.y_min;
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  Element Render() final {
    return vbox({
               main_->Render() | size(HEIGHT, EQUAL, *main_size_),
               separator() | reflect(separator_box_),
               child_->Render() | yflex,
           }) |
           reflect(box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

class ResizableSplitBottomBase : public ComponentBase {
 public:
  ResizableSplitBottomBase(Component main, Component child, int* main_size)
      : main_(std::move(main)),
        child_(std::move(child)),
        main_size_(main_size) {
    Add(Container::Vertical({
        child_,
        main_,
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

    if (captured_mouse_) {
      *main_size_ = box_.y_max - event.mouse().y;
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  Element Render() final {
    return vbox({
               child_->Render() | yflex,
               separator() | reflect(separator_box_),
               main_->Render() | size(HEIGHT, EQUAL, *main_size_),
           }) |
           reflect(box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box box_;
};

}  // namespace

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
  return Make<ResizableSplitLeftBase>(std::move(main), std::move(back),
                                      main_size);
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
/// auto split = ResizableSplitRight(right, left, &right_size);
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
  return Make<ResizableSplitRightBase>(std::move(main), std::move(back),
                                       main_size);
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
/// auto split = ResizableSplitTop(top, bottom, &top_size);
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
  return Make<ResizableSplitTopBase>(std::move(main), std::move(back),
                                     main_size);
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
/// auto split = ResizableSplit::Bottom(bottom, top, &bottom_size);
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
  return Make<ResizableSplitBottomBase>(std::move(main), std::move(back),
                                        main_size);
}
}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
