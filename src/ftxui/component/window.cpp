// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#define NOMINMAX
#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <memory>
#include <utility>
#include "ftxui/dom/elements.hpp"  // for text, window, hbox, vbox, size, clear_under, reflect, emptyElement
#include "ftxui/dom/node_decorator.hpp"  // for NodeDecorator
#include "ftxui/screen/color.hpp"        // for Color
#include "ftxui/screen/screen.hpp"       // for Screen

namespace ftxui {

namespace {

Decorator PositionAndSize(int left, int top, int width, int height) {
  return [=](Element element) {
    element |= size(WIDTH, EQUAL, width);
    element |= size(HEIGHT, EQUAL, height);

    auto padding_left = emptyElement() | size(WIDTH, EQUAL, left);
    auto padding_top = emptyElement() | size(HEIGHT, EQUAL, top);

    return vbox({
        padding_top,
        hbox({
            padding_left,
            element,
        }),
    });
  };
}

class ResizeDecorator : public NodeDecorator {
 public:
  ResizeDecorator(Element child,
                  bool resize_left,
                  bool resize_right,
                  bool resize_top,
                  bool resize_down,
                  Color color)
      : NodeDecorator(std::move(child)),
        color_(color),
        resize_left_(resize_left),
        resize_right_(resize_right),
        resize_top_(resize_top),
        resize_down_(resize_down) {}

  void Render(Screen& screen) override {
    NodeDecorator::Render(screen);

    if (resize_left_) {
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        auto& cell = screen.PixelAt(box_.x_min, y);
        cell.foreground_color = color_;
        cell.automerge = false;
      }
    }
    if (resize_right_) {
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        auto& cell = screen.PixelAt(box_.x_max, y);
        cell.foreground_color = color_;
        cell.automerge = false;
      }
    }
    if (resize_top_) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        auto& cell = screen.PixelAt(x, box_.y_min);
        cell.foreground_color = color_;
        cell.automerge = false;
      }
    }
    if (resize_down_) {
      for (int x = box_.x_min; x <= box_.x_max; ++x) {
        auto& cell = screen.PixelAt(x, box_.y_max);
        cell.foreground_color = color_;
        cell.automerge = false;
      }
    }
  }

  Color color_;
  const bool resize_left_;
  const bool resize_right_;
  const bool resize_top_;
  const bool resize_down_;
};

Element DefaultRenderState(const WindowRenderState& state) {
  Element element = state.inner;
  if (!state.active) {
    element |= dim;
  }

  element = window(text(state.title), element);
  element |= clear_under;

  const Color color = Color::Red;

  element = std::make_shared<ResizeDecorator>(  //
      element,                                  //
      state.hover_left,                         //
      state.hover_right,                        //
      state.hover_top,                          //
      state.hover_down,                         //
      color                                     //
  );

  return element;
}

class WindowImpl : public ComponentBase, public WindowOptions {
 public:
  explicit WindowImpl(WindowOptions option) : WindowOptions(std::move(option)) {
    if (!inner) {
      inner = Make<ComponentBase>();
    }
    Add(inner);
  }

 private:
  Element Render() final {
    auto element = ComponentBase::Render();

    const bool captureable =
        captured_mouse_ || ScreenInteractive::Active()->CaptureMouse();

    const WindowRenderState state = {
        element,
        title(),
        Active(),
        drag_,
        resize_left_ || resize_right_ || resize_down_ || resize_top_,
        (resize_left_hover_ || resize_left_) && captureable,
        (resize_right_hover_ || resize_right_) && captureable,
        (resize_top_hover_ || resize_top_) && captureable,
        (resize_down_hover_ || resize_down_) && captureable,
    };

    element = render ? render(state) : DefaultRenderState(state);

    // Position and record the drawn area of the window.
    element |= reflect(box_window_);
    element |= PositionAndSize(left(), top(), width(), height());
    element |= reflect(box_);

    return element;
  }

  bool OnEvent(Event event) final {
    if (ComponentBase::OnEvent(event)) {
      return true;
    }

    if (!event.is_mouse()) {
      return false;
    }

    mouse_hover_ = box_window_.Contain(event.mouse().x, event.mouse().y);

    resize_down_hover_ = false;
    resize_top_hover_ = false;
    resize_left_hover_ = false;
    resize_right_hover_ = false;

    if (mouse_hover_) {
      resize_left_hover_ = event.mouse().x == left() + box_.x_min;
      resize_right_hover_ =
          event.mouse().x == left() + width() - 1 + box_.x_min;
      resize_top_hover_ = event.mouse().y == top() + box_.y_min;
      resize_down_hover_ = event.mouse().y == top() + height() - 1 + box_.y_min;

      // Apply the component options:
      resize_top_hover_ &= resize_top();
      resize_left_hover_ &= resize_left();
      resize_down_hover_ &= resize_down();
      resize_right_hover_ &= resize_right();
    }

    if (captured_mouse_) {
      if (event.mouse().motion == Mouse::Released) {
        captured_mouse_ = nullptr;
        return true;
      }

      if (resize_left_) {
        width() = left() + width() - event.mouse().x + box_.x_min;
        left() = event.mouse().x - box_.x_min;
      }

      if (resize_right_) {
        width() = event.mouse().x - resize_start_x - box_.x_min;
      }

      if (resize_top_) {
        height() = top() + height() - event.mouse().y + box_.y_min;
        top() = event.mouse().y - box_.y_min;
      }

      if (resize_down_) {
        height() = event.mouse().y - resize_start_y - box_.y_min;
      }

      if (drag_) {
        left() = event.mouse().x - drag_start_x - box_.x_min;
        top() = event.mouse().y - drag_start_y - box_.y_min;
      }

      // Clamp the window size.
      width() = std::max<int>(width(), static_cast<int>(title().size() + 2));
      height() = std::max<int>(height(), 2);

      return true;
    }

    resize_left_ = false;
    resize_right_ = false;
    resize_top_ = false;
    resize_down_ = false;

    if (!mouse_hover_) {
      return false;
    }

    if (!CaptureMouse(event)) {
      return true;
    }

    if (event.mouse().button != Mouse::Left) {
      return true;
    }
    if (event.mouse().motion != Mouse::Pressed) {
      return true;
    }

    TakeFocus();

    captured_mouse_ = CaptureMouse(event);
    if (!captured_mouse_) {
      return true;
    }

    resize_left_ = resize_left_hover_;
    resize_right_ = resize_right_hover_;
    resize_top_ = resize_top_hover_;
    resize_down_ = resize_down_hover_;

    resize_start_x = event.mouse().x - width() - box_.x_min;
    resize_start_y = event.mouse().y - height() - box_.y_min;
    drag_start_x = event.mouse().x - left() - box_.x_min;
    drag_start_y = event.mouse().y - top() - box_.y_min;

    // Drag only if we are not resizeing a border yet:
    drag_ = !resize_right_ && !resize_down_ && !resize_top_ && !resize_left_;
    return true;
  }

  Box box_;
  Box box_window_;

  CapturedMouse captured_mouse_;
  int drag_start_x = 0;
  int drag_start_y = 0;
  int resize_start_x = 0;
  int resize_start_y = 0;

  bool mouse_hover_ = false;
  bool drag_ = false;
  bool resize_top_ = false;
  bool resize_left_ = false;
  bool resize_down_ = false;
  bool resize_right_ = false;

  bool resize_top_hover_ = false;
  bool resize_left_hover_ = false;
  bool resize_down_hover_ = false;
  bool resize_right_hover_ = false;
};

}  // namespace

/// @brief A draggeable / resizeable window. To use multiple of them, they must
/// be stacked using `Container::Stacked({...})` component;
///
/// @param option A struct holding every parameters.
/// @ingroup component
/// @see Window
///
/// ### Example
///
/// ```cpp
/// auto window_1= Window({
///     .inner = DummyWindowContent(),
///     .title = "First window",
/// });
///
/// auto window_2= Window({
///     .inner = DummyWindowContent(),
///     .title = "Second window",
/// });
///
/// auto container = Container::Stacked({
///   window_1,
///   window_2,
/// });
/// ```
Component Window(WindowOptions option) {
  return Make<WindowImpl>(std::move(option));
}

};  // namespace ftxui
