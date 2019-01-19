#include "ftxui/component/toggle.hpp"

namespace ftxui {

Element Toggle::Render() {
  bool is_focused = Focused();

  Elements children;
  for(size_t i = 0; i<entries.size(); ++i) {
    // Separator.
    if (i != 0)
      children.push_back(separator());

    // Entry.
    auto style = (selected != int(i))
                     ? normal_style
                     : is_focused ? focused_style : selected_style;
    auto focused = (selected != int(i)) ? nothing : is_focused ? focus : select;
    children.push_back(text(entries[i]) | style | focused);
  }
  return hbox(std::move(children));
}

bool Toggle::OnEvent(Event event) {
  if (selected > 0 &&
      (event == Event::ArrowLeft || event == Event::Character('h'))) {
    selected--;
    on_change();
    return true;
  }

  if (selected < int(entries.size()) - 1 &&
      (event == Event::ArrowRight || event == Event::Character('l'))) {
    selected++;
    on_change();
    return true;
  }

  return false;
}

}  // namespace ftxui
