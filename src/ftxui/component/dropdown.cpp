// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cstddef>     // for size_t
#include <functional>  // for function
#include <memory>      // for __shared_ptr_access, allocator, shared_ptr
#include <string>      // for string

#include "ftxui/component/component.hpp"  // for Maybe, Checkbox, Make, Radiobox, Vertical, Dropdown
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/component_options.hpp"  // for CheckboxOption, EntryState
#include "ftxui/dom/elements.hpp"  // for operator|, Element, border, filler, operator|=, separator, size, text, vbox, frame, vscroll_indicator, hbox, HEIGHT, LESS_THAN, bold, inverted
#include "ftxui/screen/util.hpp"   // for clamp
#include "ftxui/util/ref.hpp"      // for ConstStringListRef

namespace ftxui {

/// @brief A dropdown menu.
/// @ingroup component
/// @param entries The list of entries to display.
/// @param selected The index of the selected entry.
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
      checkbox_ = Checkbox(&title_, &show_, option);
      radiobox_ = Radiobox(entries_, selected_);

      Add(Container::Vertical({
          checkbox_,
          Maybe(radiobox_, &show_),
      }));
    }

    Element Render() override {
      *selected_ = util::clamp(*selected_, 0, int(entries_.size()) - 1);
      title_ = entries_[static_cast<size_t>(*selected_)];
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

    // Switch focus in between the checkbox and the radiobox when selecting it.
    bool OnEvent(ftxui::Event event) override {
      const bool show_old = show_;
      const int selected_old = *selected_;
      const bool handled = ComponentBase::OnEvent(event);

      if (!show_old && show_) {
        radiobox_->TakeFocus();
      }

      if (selected_old != *selected_) {
        checkbox_->TakeFocus();
        show_ = false;
      }

      return handled;
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
