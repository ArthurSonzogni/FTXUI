#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for shared_ptr, allocator
#include <string>      // for wstring, basic_string
#include <utility>     // for move

#include "ftxui/component/captured_mouse.hpp"     // for CapturedMouse
#include "ftxui/component/component.hpp"          // for Make, Input
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/deprecated.hpp"         // for Input
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Custom, Event::Delete, Event::End, Event::Home, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/deprecated.hpp"                // for text
#include "ftxui/dom/elements.hpp"  // for operator|, Element, reflect, text, dim, flex, focus, inverted, hbox, size, frame, select, underlined, Decorator, EQUAL, HEIGHT
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/string.hpp"  // for to_wstring, to_string
#include "ftxui/util/ref.hpp"  // for WideStringRef, Ref, ConstStringRef, StringRef

namespace ftxui {

// An input box. The user can type text into it.
class WideInputBase : public ComponentBase {
 public:
  WideInputBase(WideStringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option)
      : content_(content), placeholder_(placeholder), option_(option) {}

  int cursor_position_internal_ = 0;
  int& cursor_position() {
    int& opt = option_->cursor_position();
    if (opt != -1)
      return opt;
    return cursor_position_internal_;
  }

  // Component implementation:
  Element Render() override {
    std::wstring password_content;
    if (option_->password())
      password_content = std::wstring(content_->size(), U'•');
    std::wstring& content = option_->password() ? password_content : *content_;

    cursor_position() =
        std::max(0, std::min<int>(content.size(), cursor_position()));
    auto main_decorator = flex | size(HEIGHT, EQUAL, 1);
    bool is_focused = Focused();

    // placeholder.
    if (content.size() == 0) {
      if (is_focused)
        return text(*placeholder_) | focus | dim | inverted | main_decorator |
               reflect(input_box_);
      else
        return text(*placeholder_) | dim | main_decorator | reflect(input_box_);
    }

    // Not focused.
    if (!is_focused)
      return text(content) | main_decorator | reflect(input_box_);

    std::wstring part_before_cursor = content.substr(0, cursor_position());
    std::wstring part_at_cursor = cursor_position() < (int)content.size()
                                      ? content.substr(cursor_position(), 1)
                                      : L" ";
    std::wstring part_after_cursor = cursor_position() < (int)content.size() - 1
                                         ? content.substr(cursor_position() + 1)
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
      content_->insert(cursor_position(), 1, to_wstring(event.character())[0]);
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

  bool Focusable() const final { return true; }

  WideStringRef content_;
  ConstStringRef placeholder_;

  Box input_box_;
  Box cursor_box_;
  Ref<InputOption> option_;
};

// An input box. The user can type text into it.
// For convenience, the std::string version of Input simply wrap a
// WideInputBase.
// TODO(arthursonzogni): Provide an implementation handling std::string natively
// and adds better support for combining characters.
class InputBase : public WideInputBase {
 public:
  InputBase(StringRef content,
            ConstStringRef placeholder,
            Ref<InputOption> option)
      : WideInputBase(&wrapped_content_,
                      std::move(placeholder),
                      std::move(option)),
        content_(std::move(content)),
        wrapped_content_(to_wstring(*content_)) {}

  Element Render() override {
    wrapped_content_ = to_wstring(*content_);
    return WideInputBase::Render();
  }

  bool OnEvent(Event event) override {
    wrapped_content_ = to_wstring(*content_);
    if (WideInputBase::OnEvent(event)) {
      *content_ = to_string(wrapped_content_);
      return true;
    }
    return false;
  }

  StringRef content_;
  std::wstring wrapped_content_;
};

/// @brief An input box for editing text.
/// @param content The editable content.
/// @param placeholder The text displayed when content is still empty.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see InputBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::string content= "";
/// std::string placeholder = "placeholder";
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

/// @brief . An input box for editing text.
/// @param content The editable content.
/// @param placeholder The text displayed when content is still empty.
/// @param option Additional optional parameters.
/// @ingroup component
/// @see InputBase
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::FitComponent();
/// std::string content= "";
/// std::string placeholder = "placeholder";
/// Component input = Input(&content, &placeholder);
/// screen.Loop(input);
/// ```
///
/// ### Output
///
/// ```bash
/// placeholder
/// ```
Component Input(WideStringRef content,
                ConstStringRef placeholder,
                Ref<InputOption> option) {
  return Make<WideInputBase>(content, placeholder, std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
