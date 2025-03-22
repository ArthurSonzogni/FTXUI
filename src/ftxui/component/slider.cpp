// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>                              // for max, min
#include <ftxui/component/component_options.hpp>  // for SliderOption
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <string>                   // for allocator
#include <utility>                  // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Slider
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, text, Element, xflex, hbox, color, underlined, reflect, Decorator, dim, vcenter, focus, nothing, select, yflex, gaugeDirection
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color, Color::GrayDark, Color::White
#include "ftxui/util/ref.hpp"      // for ConstRef, Ref, ConstStringRef

namespace ftxui {

namespace {
Decorator flexDirection(Direction direction) {
  switch (direction) {
    case Direction::Up:
    case Direction::Down:
      return yflex;
    case Direction::Left:
    case Direction::Right:
      return xflex;
  }
  return xflex;  // NOT_REACHED()
}

template <class T>
class SliderBase : public SliderOption<T>, public ComponentBase {
 public:
  explicit SliderBase(SliderOption<T> options) : SliderOption<T>(options) {}

  Element OnRender() override {
    auto gauge_color =
        Focused() ? color(this->color_active) : color(this->color_inactive);
    const float percent =
        float(this->value() - this->min()) / float(this->max() - this->min());
    return gaugeDirection(percent, this->direction) |
           flexDirection(this->direction) | reflect(gauge_box_) | gauge_color;
  }

  void OnLeft() {
    switch (this->direction) {
      case Direction::Right:
        this->value() -= this->increment();
        break;
      case Direction::Left:
        this->value() += this->increment();
        break;
      case Direction::Up:
      case Direction::Down:
        break;
    }
  }

  void OnRight() {
    switch (this->direction) {
      case Direction::Right:
        this->value() += this->increment();
        break;
      case Direction::Left:
        this->value() -= this->increment();
        break;
      case Direction::Up:
      case Direction::Down:
        break;
    }
  }

  void OnUp() {
    switch (this->direction) {
      case Direction::Up:
        this->value() -= this->increment();
        break;
      case Direction::Down:
        this->value() += this->increment();
        break;
      case Direction::Left:
      case Direction::Right:
        break;
    }
  }

  void OnDown() {
    switch (this->direction) {
      case Direction::Down:
        this->value() += this->increment();
        break;
      case Direction::Up:
        this->value() -= this->increment();
        break;
      case Direction::Left:
      case Direction::Right:
        break;
    }
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    T old_value = this->value();
    if (event == Event::ArrowLeft || event == Event::Character('h')) {
      OnLeft();
    }
    if (event == Event::ArrowRight || event == Event::Character('l')) {
      OnRight();
    }
    if (event == Event::ArrowUp || event == Event::Character('k')) {
      OnDown();
    }
    if (event == Event::ArrowDown || event == Event::Character('j')) {
      OnUp();
    }

    this->value() = std::max(this->min(), std::min(this->max(), this->value()));
    if (old_value != this->value()) {
      if (this->on_change) {
        this->on_change();
      }
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  bool OnCapturedMouseEvent(Event event) {
    if (event.mouse().motion == Mouse::Released) {
      captured_mouse_ = nullptr;
      return true;
    }

    T old_value = this->value();
    switch (this->direction) {
      case Direction::Right: {
        this->value() = this->min() + (event.mouse().x - gauge_box_.x_min) *
                                          (this->max() - this->min()) /
                                          (gauge_box_.x_max - gauge_box_.x_min);

        break;
      }
      case Direction::Left: {
        this->value() = this->max() - (event.mouse().x - gauge_box_.x_min) *
                                          (this->max() - this->min()) /
                                          (gauge_box_.x_max - gauge_box_.x_min);
        break;
      }
      case Direction::Down: {
        this->value() = this->min() + (event.mouse().y - gauge_box_.y_min) *
                                          (this->max() - this->min()) /
                                          (gauge_box_.y_max - gauge_box_.y_min);
        break;
      }
      case Direction::Up: {
        this->value() = this->max() - (event.mouse().y - gauge_box_.y_min) *
                                          (this->max() - this->min()) /
                                          (gauge_box_.y_max - gauge_box_.y_min);
        break;
      }
    }

    this->value() = std::max(this->min(), std::min(this->max(), this->value()));

    if (old_value != this->value() && this->on_change) {
      this->on_change();
    }
    return true;
  }

  bool OnMouseEvent(Event event) {
    if (captured_mouse_) {
      return OnCapturedMouseEvent(event);
    }

    if (event.mouse().button != Mouse::Left) {
      return false;
    }
    if (event.mouse().motion != Mouse::Pressed) {
      return false;
    }

    if (!gauge_box_.Contain(event.mouse().x, event.mouse().y)) {
      return false;
    }

    captured_mouse_ = CaptureMouse(event);

    if (captured_mouse_) {
      TakeFocus();
      return OnCapturedMouseEvent(event);
    }

    return false;
  }

  bool Focusable() const final { return true; }

 private:
  Box gauge_box_;
  CapturedMouse captured_mouse_;
};

class SliderWithLabel : public ComponentBase {
 public:
  SliderWithLabel(ConstStringRef label, Component inner)
      : label_(std::move(label)) {
    Add(std::move(inner));
    SetActiveChild(ChildAt(0));
  }

 private:
  bool OnEvent(Event event) final {
    if (ComponentBase::OnEvent(event)) {
      return true;
    }

    if (!event.is_mouse()) {
      return false;
    }

    mouse_hover_ = box_.Contain(event.mouse().x, event.mouse().y);

    if (!mouse_hover_) {
      return false;
    }

    if (!CaptureMouse(event)) {
      return false;
    }

    return true;
  }

  Element OnRender() override {
    auto gauge_color = (Focused() || mouse_hover_) ? color(Color::White)
                                                   : color(Color::GrayDark);
    auto element = hbox({
                       text(label_()) | dim | vcenter,
                       hbox({
                           text("["),
                           ComponentBase::Render() | underlined,
                           text("]"),
                       }) | xflex,
                   }) |
                   gauge_color | xflex | reflect(box_);

    element |= focus;
    return element;
  }

  ConstStringRef label_;
  Box box_;
  bool mouse_hover_ = false;
};

}  // namespace

/// @brief An horizontal slider.
/// @param label The name of the slider.
/// @param value The current value of the slider.
/// @param min The minimum value.
/// @param max The maximum value.
/// @param increment The increment when used by the cursor.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// int value = 50;
/// auto slider = Slider("Value:", &value, 0, 100, 1);
/// screen.Loop(slider);
/// ```
///
/// ### Output
///
/// ```bash
/// Value:[██████████████████████████                          ]
/// ```
Component Slider(ConstStringRef label,
                 Ref<int> value,
                 ConstRef<int> min,
                 ConstRef<int> max,
                 ConstRef<int> increment) {
  SliderOption<int> option;
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  auto slider = Make<SliderBase<int>>(option);
  return Make<SliderWithLabel>(std::move(label), slider);
}

Component Slider(ConstStringRef label,
                 Ref<float> value,
                 ConstRef<float> min,
                 ConstRef<float> max,
                 ConstRef<float> increment) {
  SliderOption<float> option;
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  auto slider = Make<SliderBase<float>>(option);
  return Make<SliderWithLabel>(std::move(label), slider);
}
Component Slider(ConstStringRef label,
                 Ref<long> value,
                 ConstRef<long> min,
                 ConstRef<long> max,
                 ConstRef<long> increment) {
  SliderOption<long> option;
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  auto slider = Make<SliderBase<long>>(option);
  return Make<SliderWithLabel>(std::move(label), slider);
}

/// @brief A slider in any direction.
/// @param options The options
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// int value = 50;
/// auto slider = Slider({
///   .value = &value,
///   .min = 0,
///   .max = 100,
///   .increment= 20,
/// });
/// screen.Loop(slider);
/// ```
template <typename T>
Component Slider(SliderOption<T> options) {
  return Make<SliderBase<T>>(options);
}

template Component Slider(SliderOption<int8_t>);
template Component Slider(SliderOption<int16_t>);
template Component Slider(SliderOption<int32_t>);
template Component Slider(SliderOption<int64_t>);

template Component Slider(SliderOption<uint8_t>);
template Component Slider(SliderOption<uint16_t>);
template Component Slider(SliderOption<uint32_t>);
template Component Slider(SliderOption<uint64_t>);

template Component Slider(SliderOption<float>);
template Component Slider(SliderOption<double>);

}  // namespace ftxui
