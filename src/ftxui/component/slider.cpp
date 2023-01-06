#include <algorithm>                              // for max, min
#include <ftxui/component/component_options.hpp>  // for SliderOption
#include <string>                                 // for allocator
#include <utility>                                // for move

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Slider
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, text, GaugeDirection, Element, xflex, hbox, color, underlined, GaugeDirection::Down, GaugeDirection::Left, GaugeDirection::Right, GaugeDirection::Up, reflect, Decorator, dim, vcenter, yflex, gaugeDirection
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/color.hpp"  // for Color, Color::GrayDark, Color::White
#include "ftxui/screen/util.hpp"   // for clamp
#include "ftxui/util/ref.hpp"      // for ConstRef, Ref, ConstStringRef

namespace ftxui {

namespace {
Decorator flexDirection(GaugeDirection direction) {
  switch (direction) {
    case GaugeDirection::Up:
    case GaugeDirection::Down:
      return yflex;
    case GaugeDirection::Left:
    case GaugeDirection::Right:
      return xflex;
  }
  return xflex;  // NOT_REACHED()
}
}  // namespace

template <class T>
class SliderBase : public ComponentBase {
 public:
  explicit SliderBase(Ref<SliderOption<T>> options)
      : value_(options->value),
        min_(options->min),
        max_(options->max),
        increment_(options->increment),
        options_(options) {}

  Element Render() override {
    auto gauge_color = Focused() ? color(options_->color_active)
                                 : color(options_->color_inactive);
    const float percent = float(value_() - min_()) / float(max_() - min_());
    return gaugeDirection(percent, options_->direction) |
           flexDirection(options_->direction) | reflect(gauge_box_) |
           gauge_color;
  }

  void OnLeft() {
    switch (options_->direction) {
      case GaugeDirection::Right:
        value_() -= increment_();
        break;
      case GaugeDirection::Left:
        value_() += increment_();
        break;
      case GaugeDirection::Up:
      case GaugeDirection::Down:
        break;
    }
  }

  void OnRight() {
    switch (options_->direction) {
      case GaugeDirection::Right:
        value_() += increment_();
        break;
      case GaugeDirection::Left:
        value_() -= increment_();
        break;
      case GaugeDirection::Up:
      case GaugeDirection::Down:
        break;
    }
  }

  void OnUp() {
    switch (options_->direction) {
      case GaugeDirection::Up:
        value_() -= increment_();
        break;
      case GaugeDirection::Down:
        value_() += increment_();
        break;
      case GaugeDirection::Left:
      case GaugeDirection::Right:
        break;
    }
  }

  void OnDown() {
    switch (options_->direction) {
      case GaugeDirection::Down:
        value_() -= increment_();
        break;
      case GaugeDirection::Up:
        value_() += increment_();
        break;
      case GaugeDirection::Left:
      case GaugeDirection::Right:
        break;
    }
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    T old_value = value_();
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

    value_() = util::clamp(value_(), min_(), max_());
    if (old_value != value_()) {
      return true;
    }

    return ComponentBase::OnEvent(event);
  }

  bool OnMouseEvent(Event event) {
    if (captured_mouse_ && event.mouse().motion == Mouse::Released) {
      captured_mouse_ = nullptr;
      return true;
    }

    if (gauge_box_.Contain(event.mouse().x, event.mouse().y) &&
        CaptureMouse(event)) {
      TakeFocus();
    }

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed &&
        gauge_box_.Contain(event.mouse().x, event.mouse().y) &&
        !captured_mouse_) {
      captured_mouse_ = CaptureMouse(event);
    }

    if (captured_mouse_) {
      switch (options_->direction) {
        case GaugeDirection::Right: {
          value_() = min_() + (event.mouse().x - gauge_box_.x_min) *
                                  (max_() - min_()) /
                                  (gauge_box_.x_max - gauge_box_.x_min);
          break;
        }
        case GaugeDirection::Left: {
          value_() = max_() - (event.mouse().x - gauge_box_.x_min) *
                                  (max_() - min_()) /
                                  (gauge_box_.x_max - gauge_box_.x_min);
          break;
        }
        case GaugeDirection::Down: {
          value_() = min_() + (event.mouse().y - gauge_box_.y_min) *
                                  (max_() - min_()) /
                                  (gauge_box_.y_max - gauge_box_.y_min);
          break;
        }
        case GaugeDirection::Up: {
          value_() = max_() - (event.mouse().y - gauge_box_.y_min) *
                                  (max_() - min_()) /
                                  (gauge_box_.y_max - gauge_box_.y_min);
          break;
        }
      }
      value_() = std::max(min_(), std::min(max_(), value_()));
      return true;
    }
    return false;
  }

  bool Focusable() const final { return true; }

 private:
  Ref<T> value_;
  ConstRef<T> min_;
  ConstRef<T> max_;
  ConstRef<T> increment_;
  Ref<SliderOption<T>> options_;
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

    if (!box_.Contain(event.mouse().x, event.mouse().y)) {
      return false;
    }

    if (!CaptureMouse(event)) {
      return false;
    }

    TakeFocus();
    return true;
  }

  Element Render() override {
    auto focus_management = Focused() ? focus : Active() ? select : nothing;
    auto gauge_color = Focused() ? color(Color::White) : color(Color::GrayDark);
    return hbox({
               text(label_()) | dim | vcenter,
               hbox({
                   text("["),
                   ComponentBase::Render() | underlined,
                   text("]"),
               }) | xflex,
           }) |
           gauge_color | xflex | reflect(box_) | focus_management;
  }

  ConstStringRef label_;
  Box box_;
};

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
/// @param option The options
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

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
