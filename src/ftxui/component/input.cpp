#include <algorithm>   // for max, min
#include <cstddef>     // for size_t
#include <functional>  // for function
#include <memory>      // for shared_ptr
#include <string>      // for string, allocator
#include <utility>     // for move
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"     // for CapturedMouse
#include "ftxui/component/component.hpp"          // for Make, Input
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowLeftCtrl, Event::ArrowRight, Event::ArrowRightCtrl, Event::Backspace, Event::Custom, Event::Delete, Event::End, Event::Home, Event::Return
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Left, Mouse::Pressed
#include "ftxui/component/screen_interactive.hpp"  // for Component
#include "ftxui/dom/elements.hpp"  // for operator|, text, Element, reflect, operator|=, flex, inverted, hbox, size, bold, dim, focus, focusCursorBarBlinking, frame, select, Decorator, EQUAL, HEIGHT
#include "ftxui/screen/box.hpp"    // for Box
#include "ftxui/screen/string.hpp"  // for GlyphPosition, WordBreakProperty, GlyphCount, Utf8ToWordBreakProperty, CellToGlyphIndex, WordBreakProperty::ALetter, WordBreakProperty::CR, WordBreakProperty::Double_Quote, WordBreakProperty::Extend, WordBreakProperty::ExtendNumLet, WordBreakProperty::Format, WordBreakProperty::Hebrew_Letter, WordBreakProperty::Katakana, WordBreakProperty::LF, WordBreakProperty::MidLetter, WordBreakProperty::MidNum, WordBreakProperty::MidNumLet, WordBreakProperty::Newline, WordBreakProperty::Numeric, WordBreakProperty::Regional_Indicator, WordBreakProperty::Single_Quote, WordBreakProperty::WSegSpace, WordBreakProperty::ZWJ
#include "ftxui/screen/util.hpp"    // for clamp
#include "ftxui/util/ref.hpp"       // for StringRef, Ref, ConstStringRef

namespace ftxui {

namespace {

// Group together several propertiej so they appear to form a similar group.
// For instance, letters are grouped with number and form a single word.
bool IsWordCharacter(WordBreakProperty property) {
  switch (property) {
    case WordBreakProperty::ALetter:
    case WordBreakProperty::Hebrew_Letter:
    case WordBreakProperty::Katakana:
    case WordBreakProperty::Numeric:
      return true;

    case WordBreakProperty::CR:
    case WordBreakProperty::Double_Quote:
    case WordBreakProperty::LF:
    case WordBreakProperty::MidLetter:
    case WordBreakProperty::MidNum:
    case WordBreakProperty::MidNumLet:
    case WordBreakProperty::Newline:
    case WordBreakProperty::Single_Quote:
    case WordBreakProperty::WSegSpace:
    // Unsure:
    case WordBreakProperty::Extend:
    case WordBreakProperty::ExtendNumLet:
    case WordBreakProperty::Format:
    case WordBreakProperty::Regional_Indicator:
    case WordBreakProperty::ZWJ:
      return false;
  }
  return true;  // NOT_REACHED();
}

std::string PasswordField(size_t size) {
  std::string out;
  out.reserve(2 * size);
  while (size--) {
    out += "•";
  }
  return out;
}

// An input box. The user can type text into it.
class InputBase : public ComponentBase {
 public:
  InputBase(StringRef content,
            ConstStringRef placeholder,
            Ref<InputOption> option)
      : content_(std::move(content)),
        placeholder_(std::move(placeholder)),
        option_(std::move(option)) {}

  int cursor_position_internal_ = 0;
  int& cursor_position() {
    int& opt = option_->cursor_position();
    if (opt != -1) {
      return opt;
    }
    return cursor_position_internal_;
  }

  // Component implementation:
  Element Render() override {
    std::string password_content;
    if (option_->password()) {
      password_content = PasswordField(content_->size());
    }
    const std::string& content =
        option_->password() ? password_content : *content_;

    const int size = GlyphCount(content);

    cursor_position() = std::max(0, std::min<int>(size, cursor_position()));
    auto main_decorator = flex | ftxui::size(HEIGHT, EQUAL, 1);
    const bool is_focused = Focused();

    // placeholder.
    if (size == 0) {
      auto element = text(*placeholder_) | dim | main_decorator | reflect(box_);
      if (is_focused) {
        element |= focus;
      }
      if (hovered_ || is_focused) {
        element |= inverted;
      }
      return element;
    }

    // Not focused.
    if (!is_focused) {
      auto element = text(content) | main_decorator | reflect(box_);
      if (hovered_) {
        element |= inverted;
      }
      return element;
    }

    const int index_before_cursor = GlyphPosition(content, cursor_position());
    const int index_after_cursor =
        GlyphPosition(content, 1, index_before_cursor);
    const std::string part_before_cursor =
        content.substr(0, index_before_cursor);
    std::string part_at_cursor = " ";
    if (cursor_position() < size) {
      part_at_cursor = content.substr(index_before_cursor,
                                      index_after_cursor - index_before_cursor);
    }
    const std::string part_after_cursor = content.substr(index_after_cursor);
    auto focused = (is_focused || hovered_) ? focusCursorBarBlinking : select;
    return hbox({
               text(part_before_cursor),
               text(part_at_cursor) | focused | reflect(cursor_box_),
               text(part_after_cursor),
           }) |
           flex | frame | bold | main_decorator | reflect(box_);
  }

  bool OnEvent(Event event) override {
    cursor_position() =
        std::max(0, std::min<int>((int)content_->size(), cursor_position()));

    if (event.is_mouse()) {
      return OnMouseEvent(event);
    }

    // Backspace.
    if (event == Event::Backspace) {
      if (cursor_position() == 0) {
        return false;
      }
      const size_t start = GlyphPosition(*content_, cursor_position() - 1);
      const size_t end = GlyphPosition(*content_, cursor_position());
      content_->erase(start, end - start);
      cursor_position()--;
      option_->on_change();
      return true;
    }

    // Delete
    if (event == Event::Delete) {
      if (cursor_position() == int(content_->size())) {
        return false;
      }
      const size_t start = GlyphPosition(*content_, cursor_position());
      const size_t end = GlyphPosition(*content_, cursor_position() + 1);
      content_->erase(start, end - start);
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

    // Arrow
    if (event == Event::ArrowLeft && cursor_position() > 0) {
      cursor_position()--;
      return true;
    }

    if (event == Event::ArrowRight &&
        cursor_position() < (int)content_->size()) {
      cursor_position()++;
      return true;
    }

    // CTRL + Arrow:
    if (event == Event::ArrowLeftCtrl) {
      HandleLeftCtrl();
      return true;
    }
    if (event == Event::ArrowRightCtrl) {
      HandleRightCtrl();
      return true;
    }

    if (event == Event::Home) {
      cursor_position() = 0;
      return true;
    }

    if (event == Event::End) {
      cursor_position() = GlyphCount(*content_);
      return true;
    }

    // Content
    if (event.is_character()) {
      const size_t start = GlyphPosition(*content_, cursor_position());
      content_->insert(start, event.character());
      cursor_position()++;
      option_->on_change();
      return true;
    }
    return false;
  }

 private:
  void HandleLeftCtrl() {
    auto properties = Utf8ToWordBreakProperty(*content_);

    // Move left, as long as left is not a word character.
    while (cursor_position() > 0 &&
           !IsWordCharacter(properties[cursor_position() - 1])) {
      cursor_position()--;
    }

    // Move left, as long as left is a word character:
    while (cursor_position() > 0 &&
           IsWordCharacter(properties[cursor_position() - 1])) {
      cursor_position()--;
    }
  }

  void HandleRightCtrl() {
    auto properties = Utf8ToWordBreakProperty(*content_);
    const int max = (int)properties.size();

    // Move right, as long as right is not a word character.
    while (cursor_position() < max &&
           !IsWordCharacter(properties[cursor_position()])) {
      cursor_position()++;
    }

    // Move right, as long as right is a word character:
    while (cursor_position() < max &&
           IsWordCharacter(properties[cursor_position()])) {
      cursor_position()++;
    }
  }

  bool OnMouseEvent(Event event) {
    hovered_ =
        box_.Contain(event.mouse().x, event.mouse().y) && CaptureMouse(event);
    if (!hovered_) {
      return false;
    }

    if (event.mouse().button != Mouse::Left ||
        event.mouse().motion != Mouse::Pressed) {
      return false;
    }

    TakeFocus();
    if (content_->empty()) {
      return true;
    }

    auto mapping = CellToGlyphIndex(*content_);
    int original_glyph = cursor_position();
    original_glyph = util::clamp(original_glyph, 0, int(mapping.size()));
    size_t original_cell = 0;
    for (size_t i = 0; i < mapping.size(); i++) {
      if (mapping[i] == original_glyph) {
        original_cell = (int)i;
        break;
      }
    }
    if (mapping[original_cell] != original_glyph) {
      original_cell = mapping.size();
    }
    const int target_cell =
        int(original_cell) + event.mouse().x - cursor_box_.x_min;
    int target_glyph = target_cell < (int)mapping.size() ? mapping[target_cell]
                                                         : (int)mapping.size();
    target_glyph = util::clamp(target_glyph, 0, GlyphCount(*content_));
    if (cursor_position() != target_glyph) {
      cursor_position() = target_glyph;
      option_->on_change();
    }
    return true;
  }

  bool Focusable() const final { return true; }

  bool hovered_ = false;
  StringRef content_;
  ConstStringRef placeholder_;

  Box box_;
  Box cursor_box_;
  Ref<InputOption> option_;
};

}  // namespace

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
  return Make<InputBase>(std::move(content), std::move(placeholder),
                         std::move(option));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
