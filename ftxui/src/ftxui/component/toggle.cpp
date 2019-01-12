#include "ftxui/component/toggle.hpp"

namespace ftxui {

Element Toggle::Render() {
  auto highlight = Focused() ? inverted : bold;

  Elements children;
  for(size_t i = 0; i<entries.size(); ++i) {
    // Separator.
    if (i != 0)
      children.push_back(separator());

    // Entry.
    auto style = i == selected ? highlight : dim;
    children.push_back(style(text(entries[i])));
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

  if (selected < entries.size() - 1 &&
      (event == Event::ArrowRight || event == Event::Character('l'))) {
    selected++;
    on_change();
    return true;
  }

  return false;
}

}  // namespace ftxui
