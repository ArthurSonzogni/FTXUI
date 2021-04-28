#include "ftxui/component/slider.hpp"

namespace ftxui {
class SliderInt : public Component {
 public:
  SliderInt(std::wstring label, int* value, int min, int max, int increment)
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
    if (!box_.Contain(event.mouse().x, event.mouse().y))
      return false;
    TakeFocus();
    if (!gauge_box_.Contain(event.mouse().x, event.mouse().y))
      return false;
    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      *value_ = min_ + (event.mouse().x - gauge_box_.x_min) * (max_ - min_) /
                          (gauge_box_.x_max - gauge_box_.x_min);
    }
    return true;
  }

 private:
  std::wstring label_;
  int* value_;
  int min_;
  int max_;
  int increment_ = 1;
  Box box_;
  Box gauge_box_;
};

ComponentPtr Slider(std::wstring label,
                    int* value,
                    int min,
                    int max,
                    int increment) {
  return std::make_unique<SliderInt>(std::move(label), value, min, max,
                                     increment);
}

}  // namespace ftxui
