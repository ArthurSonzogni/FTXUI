// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/event.hpp>
#include <functional>  // for function
#include <string>      // for string

#include <utility>
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
  DropdownOption option;
  option.radiobox.entries = std::move(entries);
  option.radiobox.selected = selected;
  return Dropdown(option);
}

/// @brief A dropdown menu.
/// @ingroup component
/// @param option The options for the dropdown.
// NOLINTNEXTLINE
Component Dropdown(DropdownOption option) {
  class Impl : public ComponentBase, public DropdownOption {
   public:
    explicit Impl(DropdownOption option) : DropdownOption(std::move(option)) {
      FillDefault();
      checkbox_ = Checkbox(checkbox);
      radiobox_ = Radiobox(radiobox);

      Add(Container::Vertical({
          checkbox_,
          Maybe(radiobox_, checkbox.checked),
      }));
    }

    Element Render() override {
      radiobox.selected =
          util::clamp(radiobox.selected(), 0, int(radiobox.entries.size()) - 1);
      title_ = radiobox.entries[selected_()];

      return transform(*open_, checkbox_->Render(), radiobox_->Render());
    }

    // Switch focus in between the checkbox and the radiobox when selecting it.
    bool OnEvent(ftxui::Event event) override {
      const bool open_old = open_();
      const int selected_old = selected_();
      bool handled = ComponentBase::OnEvent(event);

      // Transfer focus to the radiobox when the dropdown is opened.
      if (!open_old && open_()) {
        radiobox_->TakeFocus();
      }

      // Auto-close the dropdown when the user selects an item, even if the item
      // it the same as the previous one.
      if (open_old && open_()) {
        const bool should_close = (selected_() != selected_old) ||     //
                                  (event == Event::Return) ||          //
                                  (event == Event::Character(' ')) ||  //
                                  (event == Event::Escape);            //

        if (should_close) {
          checkbox_->TakeFocus();
          open_ = false;
          handled = true;
        }
      }

      return handled;
    }

    void FillDefault() {
      open_ = checkbox.checked;
      selected_ = radiobox.selected;
      checkbox.checked = &*open_;
      radiobox.selected = &*selected_;
      checkbox.label = &title_;

      if (!checkbox.transform) {
        checkbox.transform = [](const EntryState& s) {
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
      }

      if (!transform) {
        transform = [](bool is_open, Element checkbox_element,
                       Element radiobox_element) {
          if (is_open) {
            const int max_height = 12;
            return vbox({
                       std::move(checkbox_element),
                       separator(),
                       std::move(radiobox_element) | vscroll_indicator | frame |
                           size(HEIGHT, LESS_THAN, max_height),
                   }) |
                   border;
          }
          return vbox({std::move(checkbox_element), filler()}) | border;
        };
      }
    }

   private:
    Ref<bool> open_;
    Ref<int> selected_;
    Component checkbox_;
    Component radiobox_;
    std::string title_;
  };

  return Make<Impl>(option);
}

}  // namespace ftxui
