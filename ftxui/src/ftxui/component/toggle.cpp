#include "ftxui/component/toggle.hpp"

namespace ftxui {
namespace component {

Toggle::Toggle(Delegate* delegate) : Component(delegate) {}

dom::Element Toggle::Render() {
  using namespace dom;
  auto highlight = Focused() ? inverted : bold;

  Children children;

  for(size_t i = 0; i<options.size(); ++i) {
    // Separator.
    if (i != 0)
      children.push_back(separator());

    // Entry.
    auto style = i == activated ? highlight : dim;
    children.push_back(style(text(options[i])));
  }
  return hbox(std::move(children));
}

bool Toggle::OnEvent(Event event) {
  if (activated > 0 &&
      (event == Event::ArrowLeft || event == Event::Character('h'))) {
    activated--;
    on_change();
    return true;
  }

  if (activated < options.size() - 1 &&
      (event == Event::ArrowRight || event == Event::Character('l'))) {
    activated++;
    on_change();
    return true;
  }

  return false;
}

}  // namespace component
}  // namespace ftxui
