// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>                              // for max, min
#include <ftxui/component/component_options.hpp>  // for SliderOption
#include <ftxui/dom/direction.hpp>  // for Direction, Direction::Down, Direction::Left, Direction::Right, Direction::Up
#include <utility>                  // for move

#include "ftxui/component/app.hpp"             // for Component
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Make, Slider
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed, Mouse::Released
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

Direction Opposite(Direction d) {
  switch (d) {
    case Direction::Up:
      return Direction::Down;
    case Direction::Down:
      return Direction::Up;
    case Direction::Left:
      return Direction::Right;
    case Direction::Right:
      return Direction::Left;
  }
  return d;  // NOT_REACHED()
}

template <class T>
class SliderBase : public SliderOption<T>, public ComponentBase {
 public:
  explicit SliderBase(SliderOption<T> options)
      : SliderOption<T>(std::move(options)) {}

  Element OnRender() override {
    auto gauge_color = (Focused() || hovered_)
                           ? color(this->color_active)
                           : color(this->color_inactive);
    const float percent =
        float(this->value() - this->min()) / float(this->max() - this->min());
    auto gauge = gaugeDirection(percent, this->direction) |
                 flexDirection(this->direction) | reflect(gauge_box_) |
                 gauge_color;

    if (this->label().empty()) {
      return gauge | reflect(box_);
    }

    if (this->direction == Direction::Right ||
        this->direction == Direction::Left) {
      auto element = hbox({
                         text(this->label()) | dim | vcenter,
                         hbox({
                             text("["),
                             gauge | underlined,
                             text("]"),
                         }) | vcenter |
                             xflex,
                     }) |
                     gauge_color | xflex | reflect(box_);
      element |= focus;
      return element;
    }

    auto element = vbox({
                       text(this->label()) | dim | hcenter,
                       gauge,
                   }) |
                   gauge_color | yflex | reflect(box_);
    element |= focus;
    return element;
  }

  void OnDirection(Direction pressed) {
    if (pressed == this->direction) {
      this->value() += this->increment();
      return;
    }

    if (pressed == Opposite(this->direction)) {
      this->value() -= this->increment();
      return;
    }
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    T old_value = this->value();
    if (event == Event::ArrowLeft || event == Event::Character('h')) {
      OnDirection(Direction::Left);
    }
    if (event == Event::ArrowRight || event == Event::Character('l')) {
      OnDirection(Direction::Right);
    }
    if (event == Event::ArrowUp || event == Event::Character('k')) {
      OnDirection(Direction::Up);
    }
    if (event == Event::ArrowDown || event == Event::Character('j')) {
      OnDirection(Direction::Down);
    }

    this->value() = std::max(this->min(), std::min(this->max(), this->value()));
    if (old_value != this->value()) {
      App::PostEventOrExecute(this->on_change);
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

    if (old_value != this->value()) {
      App::PostEventOrExecute(this->on_change);
    }
    return true;
  }

  bool OnMouseEvent(Event event) {
    if (captured_mouse_) {
      return OnCapturedMouseEvent(event);
    }

    hovered_ = box_.Contain(event.mouse().x, event.mouse().y);

    if (event.mouse().button != Mouse::Left) {
      return false;
    }
    if (event.mouse().motion != Mouse::Pressed) {
      return false;
    }

    if (gauge_box_.Contain(event.mouse().x, event.mouse().y)) {
      captured_mouse_ = CaptureMouse(event);
      if (captured_mouse_) {
        TakeFocus();
        return OnCapturedMouseEvent(event);
      }
    }

    if (box_.Contain(event.mouse().x, event.mouse().y)) {
      TakeFocus();
      return true;
    }

    return false;
  }

  bool Focusable() const final { return true; }

 private:
  Box gauge_box_;
  Box box_;
  bool hovered_ = false;
  CapturedMouse captured_mouse_;
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
/// auto screen = App::TerminalOutput();
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
  option.label = std::move(label);
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  return Make<SliderBase<int>>(std::move(option));
}

Component Slider(ConstStringRef label,
                 Ref<float> value,
                 ConstRef<float> min,
                 ConstRef<float> max,
                 ConstRef<float> increment) {
  SliderOption<float> option;
  option.label = std::move(label);
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  return Make<SliderBase<float>>(std::move(option));
}
Component Slider(ConstStringRef label,
                 Ref<long> value,
                 ConstRef<long> min,
                 ConstRef<long> max,
                 ConstRef<long> increment) {
  SliderOption<long> option;
  option.label = std::move(label);
  option.value = value;
  option.min = min;
  option.max = max;
  option.increment = increment;
  return Make<SliderBase<long>>(std::move(option));
}

/// @brief A slider in any direction.
/// @param options The options
/// ### Example
///
/// ```cpp
/// auto screen = App::TerminalOutput();
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
  return Make<SliderBase<T>>(std::move(options));
}

template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int8_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int16_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int32_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<int64_t>);

template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<uint8_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<uint16_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<uint32_t>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<uint64_t>);

template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<float>);
template FTXUI_EXPORT(COMPONENT) Component Slider(SliderOption<double>);

}  // namespace ftxui
