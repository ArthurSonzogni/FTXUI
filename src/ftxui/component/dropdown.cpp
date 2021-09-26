#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"

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
