#include <algorithm>  // for max, min
#include <memory>     // for shared_ptr
#include <string>     // for wstring, allocator, basic_string

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/component.hpp"       // for Component
#include "ftxui/component/component_base.hpp"  // for Component
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Custom, Event::Delete, Event::End, Event::Home, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/util/ref.hpp"                      // for Component

namespace ftxui {

/// @brief An input box. The user can type text into it.
/// @ingroup component.
class InputBase : public ComponentBase {
 public:
  InputBase(StringRef content,
            ConstStringRef placeholder,
            Ref<InputOption> option)
      : content_(content), placeholder_(placeholder), option_(option) {}
  ~InputBase() override = default;

  int& cursor_position() { return *(option_->cursor_position); }

  // Component implementation:
  Element Render() override {
    cursor_position() =
        std::max(0, std::min<int>(content_->size(), cursor_position()));
    auto main_decorator = flex | size(HEIGHT, EQUAL, 1);
    bool is_focused = Focused();

    // placeholder.
    if (content_->size() == 0) {
      if (is_focused)
        return text(*placeholder_) | focus | dim | inverted | main_decorator |
               reflect(input_box_);
      else
        return text(*placeholder_) | dim | main_decorator | reflect(input_box_);
    }

    // Not focused.
    if (!is_focused)
      return text(*content_) | main_decorator | reflect(input_box_);

    std::wstring part_before_cursor = content_->substr(0, cursor_position());
    std::wstring part_at_cursor = cursor_position() < (int)content_->size()
                                      ? content_->substr(cursor_position(), 1)
                                      : L" ";
    std::wstring part_after_cursor =
        cursor_position() < (int)content_->size() - 1
            ? content_->substr(cursor_position() + 1)
            : L"";
    auto focused = is_focused ? focus : select;

    // clang-format off
  return
    hbox(
      text(part_before_cursor),
      text(part_at_cursor) | underlined | focused | reflect(cursor_box_),
      text(part_after_cursor)
    ) | flex | inverted | frame | main_decorator | reflect(input_box_);
    // clang-format on
  }

  bool OnEvent(Event event) override {
    cursor_position() =
        std::max(0, std::min<int>(content_->size(), cursor_position()));

    if (event.is_mouse())
      return OnMouseEvent(event);

    std::wstring c;

    // Backspace.
    if (event == Event::Backspace) {
      if (cursor_position() == 0)
        return false;
      content_->erase(cursor_position() - 1, 1);
      cursor_position()--;
      option_->on_change();
      return true;
    }

    // Delete
    if (event == Event::Delete) {
      if (cursor_position() == int(content_->size()))
        return false;
      content_->erase(cursor_position(), 1);
      option_->on_change();
      return true;
    }

    // Enter.
    if (event == Event::Return) {
      option_->on_enter();
      return true;
    }

    if (event == Event::Custom) {
      return false;
    }

    if (event == Event::ArrowLeft && cursor_position() > 0) {
      cursor_position()--;
      return true;
    }

    if (event == Event::ArrowRight &&
        cursor_position() < (int)content_->size()) {
      cursor_position()++;
      return true;
    }

    if (event == Event::Home) {
      cursor_position() = 0;
      return true;
    }

    if (event == Event::End) {
      cursor_position() = (int)content_->size();
      return true;
    }

    // Content
    if (event.is_character()) {
      content_->insert(cursor_position(), 1, event.character());
      cursor_position()++;
      option_->on_change();
      return true;
    }
    return false;
  }

 private:
  bool OnMouseEvent(Event event) {
    if (!CaptureMouse(event))
      return false;
    if (!input_box_.Contain(event.mouse().x, event.mouse().y))
      return false;

    TakeFocus();

    if (event.mouse().button == Mouse::Left &&
        event.mouse().motion == Mouse::Pressed) {
      int new_cursor_position =
          cursor_position() + event.mouse().x - cursor_box_.x_min;
      new_cursor_position =
          std::max(0, std::min<int>(content_->size(), new_cursor_position));
      if (cursor_position() != new_cursor_position) {
        cursor_position() = new_cursor_position;
        option_->on_change();
      }
    }
    return true;
  }
  StringRef content_;
  ConstStringRef placeholder_;

  Box input_box_;
  Box cursor_box_;
  Ref<InputOption> option_;
};

/// @brief An input box for editing text.
/// @param content The editable content.
/// @param placeholder The text displayed when content is still empty.
/// @ingroup component
/// @see InputBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::wstring content= L"";
/// std::wstring placeholder = L"placeholder";
/// Component input = Input(&content, &placeholder);
/// screen.Loop(input);
/// ```
///
/// ### Output
///
/// ```bash
/// placeholder
/// ```
Component Input(StringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option) {
  return Make<InputBase>(content, placeholder, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
