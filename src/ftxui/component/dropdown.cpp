#include <algorithm>   // for max, min
#include <functional>  // for function
#include <memory>      // for __shared_ptr_access, shared_ptr, allocator
#include <string>      // for string

#include "ftxui/component/component.hpp"  // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption, EntryState
#include "ftxui/dom/elements.hpp"  // for operator|, Element, border, filler, operator|=, separator, size, text, vbox, frame, vscroll_indicator, hbox, HEIGHT, LESS_THAN, bold, inverted
#include "ftxui/util/ref.hpp"      // for ConstStringListRef

namespace ftxui {

Component Dropdown(ConstStringListRef entries, int* selected) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringListRef entries, int* selected)
        : entries_(entries), selected_(selected) {
      CheckboxOption option;
      option.transform = [](const EntryState& s) {
        auto prefix = text(s.state ? "↓ " : "→ ");  // NOLINT
        auto t = text(s.label);
        if (s.active) {
          t |= bold;
        }
        if (s.focused) {
          t |= inverted;
        }
        return hbox({prefix, t});
      };
      checkbox_ = Checkbox(&title_, &show_, option),
      radiobox_ = Radiobox(entries_, selected_);

      Add(Container::Vertical({
          checkbox_,
          Maybe(radiobox_, &show_),
      }));
    }

    Element Render() override {
      *selected_ = std::min((int)entries_.size() - 1, std::max(0, *selected_));
      title_ = entries_[*selected_];
      if (show_) {
        const int max_height = 12;
        return vbox({
                   checkbox_->Render(),
                   separator(),
                   radiobox_->Render() | vscroll_indicator | frame |
                       size(HEIGHT, LESS_THAN, max_height),
               }) |
               border;
      }

      return vbox({
          checkbox_->Render() | border,
          filler(),
      });
    }

   private:
    ConstStringListRef entries_;
    bool show_ = false;
    int* selected_;
    std::string title_;
    Component checkbox_;
    Component radiobox_;
  };

  return Make<Impl>(entries, selected);
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
