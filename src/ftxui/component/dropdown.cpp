#include <algorithm>  // for max, min
#include <memory>     // for __shared_ptr_access
#include <string>     // for string
#include <utility>    // for move

#include "ftxui/component/component.hpp"  // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption
#include "ftxui/dom/elements.hpp"  // for operator|, Element, border, filler, separator, size, vbox, frame, vscroll_indicator, HEIGHT, LESS_THAN
#include "ftxui/util/ref.hpp"      // for ConstStringListRef

namespace ftxui {

Component Dropdown(ConstStringListRef entries, int* selected) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringListRef entries, int* selected)
        : entries_(std::move(entries)), selected_(selected) {
      CheckboxOption option;
      option.style_checked = "↓│";
      option.style_unchecked = "→│";
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
        return vbox({
                   checkbox_->Render(),
                   separator(),
                   radiobox_->Render() | vscroll_indicator | frame |
                       size(HEIGHT, LESS_THAN, 12),
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

  return Make<Impl>(std::move(entries), selected);
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
