#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace {

class ResizableSplitLeft : public ComponentBase {
 public:
  ResizableSplitLeft(Component main, Component child, int* main_size)
      : main_(main), child_(child), main_size_(main_size) {
    Add(Container::Horizontal({
        main,
        child,
    }));
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse())
      return OnMouseEvent(std::move(event));
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
      *main_size_ = event.mouse().x - global_box_.x_min;
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
           reflect(global_box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box global_box_;
};

class ResizableSplitRight: public ComponentBase {
 public:
  ResizableSplitRight(Component main, Component child, int* main_size)
      : main_(main), child_(child), main_size_(main_size) {
    Add(Container::Horizontal({
        child,
        main,
    }));
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse())
      return OnMouseEvent(std::move(event));
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
      *main_size_ = global_box_.x_max - event.mouse().x;
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
           reflect(global_box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box global_box_;
};

class ResizableSplitTop: public ComponentBase {
 public:
  ResizableSplitTop(Component main, Component child, int* main_size)
      : main_(main), child_(child), main_size_(main_size) {
    Add(Container::Vertical({
        main,
        child,
    }));
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse())
      return OnMouseEvent(std::move(event));
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
      *main_size_ = event.mouse().y - global_box_.y_min;
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
           reflect(global_box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box global_box_;
};

class ResizableSplitBottom: public ComponentBase {
 public:
  ResizableSplitBottom(Component main, Component child, int* main_size)
      : main_(main), child_(child), main_size_(main_size) {
    Add(Container::Vertical({
        child,
        main,
    }));
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse())
      return OnMouseEvent(std::move(event));
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
      *main_size_ = global_box_.y_max - event.mouse().y;
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
           reflect(global_box_);
  };

 private:
  Component main_;
  Component child_;
  int* const main_size_;
  CapturedMouse captured_mouse_;
  Box separator_box_;
  Box global_box_;
};

}  // namespace

namespace ResizableSplit {
Component Left(Component main, Component back, int* main_size) {
  return Make<ResizableSplitLeft>(std::move(main), std::move(back), main_size);
}
Component Right(Component main, Component back, int* main_size) {
  return Make<ResizableSplitRight>(std::move(main), std::move(back), main_size);
}
Component Top(Component main, Component back, int* main_size) {
  return Make<ResizableSplitTop>(std::move(main), std::move(back), main_size);
}
Component Bottom(Component main, Component back, int* main_size) {
  return Make<ResizableSplitBottom>(std::move(main), std::move(back), main_size);
}
//Component Top(Component main, Component back, int main_size) {
  //return Make<ResizableSplitTop>(std::move(main), std::move(back), main_size);
//}
//Component Bottom(Component main, Component back, int main_size) {
  //return Make<ResizableSplitBottom>(std::move(main), std::move(back),
                                    //main_size);
//}
}  // namespace ResizableSplit
}  // namespace ftxui
