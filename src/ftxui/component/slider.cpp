#include "ftxui/component/slider.hpp"

#include <memory>
#include <utility>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/box.hpp"
#include "ftxui/screen/color.hpp"

namespace ftxui {

template <class T>
class SliderImpl : public Component {
 public:
  SliderImpl(std::wstring label, T* value, T min, T max, T increment)
      : label_(label),
        value_(value),
        min_(min),
        max_(max),
        increment_(increment) {}

  Element Render() {
    auto gauge_color =
        Focused() ? color(Color::GrayLight) : color(Color::GrayDark);
    float percent = float(*value_ - min_) / float(max_ - min_);
    return hbox({
               text(label_) | dim | vcenter,
               hbox({
                   text(L"["),
                   gauge(percent) | underlined | xflex | reflect(gauge_box_),
                   text(L"]"),
               }) | xflex,
           }) |
           gauge_color | xflex | reflect(box_);
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse())
      return OnMouseEvent(event);

    if (event == Event::ArrowLeft || event == Event::Character('h')) {
      *value_ -= increment_;
      *value_ = std::max(*value_, min_);
      return true;
    }

    if (event == Event::ArrowRight || event == Event::Character('l')) {
      *value_ += increment_;
      *value_ = std::min(*value_, max_);
      return true;
    }

    return Component::OnEvent(event);
  }

  bool OnMouseEvent(Event event) {
    if (captured_mouse_ && event.mouse().motion == Mouse::Released) {
      captured_mouse_ = nullptr;
      return true;
    }

    if (box_.Contain(event.mouse().x, event.mouse().y) &&
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
      *value_ = min_ + (event.mouse().x - gauge_box_.x_min) * (max_ - min_) /
                           (gauge_box_.x_max - gauge_box_.x_min);
      *value_ = std::max(min_, std::min(max_, *value_));
      return true;
    }
    return false;
  }

 private:
  std::wstring label_;
  T* value_;
  T min_;
  T max_;
  T increment_ = 1;
  Box box_;
  Box gauge_box_;
  CapturedMouse captured_mouse_;
};

template <class T>
ComponentPtr Slider(std::wstring label, T* value, T min, T max, T increment) {
  return std::make_unique<SliderImpl<T>>(std::move(label), value, min, max,
                                         increment);
}

template ComponentPtr Slider(std::wstring label,
                             int* value,
                             int min,
                             int max,
                             int increment);

template ComponentPtr Slider(std::wstring label,
                             float* value,
                             float min,
                             float max,
                             float increment);

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
