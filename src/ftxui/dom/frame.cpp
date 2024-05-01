// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for max, min
#include <memory>     // for make_shared, __shared_ptr_access
#include <utility>    // for move

#include "ftxui/dom/elements.hpp"  // for Element, unpack, Elements, focus, frame, select, xframe, yframe
#include "ftxui/dom/node.hpp"  // for Node, Elements
#include "ftxui/dom/requirement.hpp"  // for Requirement, Requirement::FOCUSED, Requirement::SELECTED
#include "ftxui/screen/box.hpp"      // for Box
#include "ftxui/screen/screen.hpp"   // for Screen, Screen::Cursor
#include "ftxui/util/autoreset.hpp"  // for AutoReset

namespace ftxui {

namespace {
class Select : public Node {
 public:
  explicit Select(Elements children) : Node(std::move(children)) {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
    auto& selected_box = requirement_.selected_box;
    selected_box.x_min = 0;
    selected_box.y_min = 0;
    selected_box.x_max = requirement_.min_x - 1;
    selected_box.y_max = requirement_.min_y - 1;
    requirement_.selection = Requirement::SELECTED;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    children_[0]->SetBox(box);
  }
};

class Focus : public Select {
 public:
  using Select::Select;

  void ComputeRequirement() override {
    Select::ComputeRequirement();
    requirement_.selection = Requirement::FOCUSED;
  }

  void Render(Screen& screen) override {
    Select::Render(screen);

    // Setting the cursor to the right position allow folks using CJK (China,
    // Japanese, Korean, ...) characters to see their [input method editor]
    // displayed at the right location. See [issue].
    //
    // [input method editor]:
    // https://en.wikipedia.org/wiki/Input_method
    //
    // [issue]:
    // https://github.com/ArthurSonzogni/FTXUI/issues/2#issuecomment-505282355
    //
    // Unfortunately, Microsoft terminal do not handle properly hidding the
    // cursor. Instead the character under the cursor is hidden, which is a big
    // problem. As a result, we can't enable setting cursor to the right
    // location. It will be displayed at the bottom right corner.
    // See:
    // https://github.com/microsoft/terminal/issues/1203
    // https://github.com/microsoft/terminal/issues/3093
#if !defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
    screen.SetCursor(Screen::Cursor{
        box_.x_min,
        box_.y_min,
        Screen::Cursor::Shape::Hidden,
    });
#endif
  }
};

class Frame : public Node {
 public:
  Frame(Elements children, bool x_frame, bool y_frame)
      : Node(std::move(children)), x_frame_(x_frame), y_frame_(y_frame) {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children_[0]->requirement();
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    auto& selected_box = requirement_.selected_box;
    Box children_box = box;

    if (x_frame_) {
      const int external_dimx = box.x_max - box.x_min;
      const int internal_dimx = std::max(requirement_.min_x, external_dimx);
      const int focused_dimx = selected_box.x_max - selected_box.x_min;
      int dx = selected_box.x_min - external_dimx / 2 + focused_dimx / 2;
      dx = std::max(0, std::min(internal_dimx - external_dimx - 1, dx));
      children_box.x_min = box.x_min - dx;
      children_box.x_max = box.x_min + internal_dimx - dx;
    }

    if (y_frame_) {
      const int external_dimy = box.y_max - box.y_min;
      const int internal_dimy = std::max(requirement_.min_y, external_dimy);
      const int focused_dimy = selected_box.y_max - selected_box.y_min;
      int dy = selected_box.y_min - external_dimy / 2 + focused_dimy / 2;
      dy = std::max(0, std::min(internal_dimy - external_dimy - 1, dy));
      children_box.y_min = box.y_min - dy;
      children_box.y_max = box.y_min + internal_dimy - dy;
    }

    children_[0]->SetBox(children_box);
  }

  void Render(Screen& screen) override {
    const AutoReset<Box> stencil(&screen.stencil,
                                 Box::Intersection(box_, screen.stencil));
    children_[0]->Render(screen);
  }

 private:
  bool x_frame_;
  bool y_frame_;
};

class FocusCursor : public Focus {
 public:
  FocusCursor(Elements children, Screen::Cursor::Shape shape)
      : Focus(std::move(children)), shape_(shape) {}

 private:
  void Render(Screen& screen) override {
    Select::Render(screen);  // NOLINT
    screen.SetCursor(Screen::Cursor{
        box_.x_min,
        box_.y_min,
        shape_,
    });
  }
  Screen::Cursor::Shape shape_;
};

}  // namespace

/// @brief Set the `child` to be the one selected among its siblings.
/// @param child The element to be selected.
/// @ingroup dom
Element select(Element child) {
  return std::make_shared<Select>(unpack(std::move(child)));
}

/// @brief Set the `child` to be the one in focus globally.
/// @param child The element to be focused.
/// @ingroup dom
Element focus(Element child) {
  return std::make_shared<Focus>(unpack(std::move(child)));
}

/// @brief Allow an element to be displayed inside a 'virtual' area. It size can
/// be larger than its container. In this case only a smaller portion is
/// displayed. The view is scrollable to make the focused element visible.
/// @see frame
/// @see xframe
/// @see yframe
Element frame(Element child) {
  return std::make_shared<Frame>(unpack(std::move(child)), true, true);
}

/// @brief Same as `frame`, but only on the x-axis.
/// @see frame
/// @see xframe
/// @see yframe
Element xframe(Element child) {
  return std::make_shared<Frame>(unpack(std::move(child)), true, false);
}

/// @brief Same as `frame`, but only on the y-axis.
/// @see frame
/// @see xframe
/// @see yframe
Element yframe(Element child) {
  return std::make_shared<Frame>(unpack(std::move(child)), false, true);
}

/// @brief Same as `focus`, but set the cursor shape to be a still block.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorBlock(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::Block);
}

/// @brief Same as `focus`, but set the cursor shape to be a blinking block.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorBlockBlinking(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::BlockBlinking);
}

/// @brief Same as `focus`, but set the cursor shape to be a still block.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorBar(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::Bar);
}

/// @brief Same as `focus`, but set the cursor shape to be a blinking bar.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorBarBlinking(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::BarBlinking);
}

/// @brief Same as `focus`, but set the cursor shape to be a still underline.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorUnderline(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::Underline);
}

/// @brief Same as `focus`, but set the cursor shape to be a blinking underline.
/// @see focus
/// @see focusCursorBlock
/// @see focusCursorBlockBlinking
/// @see focusCursorBar
/// @see focusCursorBarBlinking
/// @see focusCursorUnderline
/// @see focusCursorUnderlineBlinking
/// @ingroup dom
Element focusCursorUnderlineBlinking(Element child) {
  return std::make_shared<FocusCursor>(unpack(std::move(child)),
                                       Screen::Cursor::UnderlineBlinking);
}

}  // namespace ftxui
