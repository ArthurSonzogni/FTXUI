#include "ftxui/component/toggle.hpp"

namespace ftxui {
namespace component {

Toggle::Toggle(Delegate* delegate) : Component(delegate) {}

dom::Element Toggle::Render() {
  using namespace dom;
  auto highlight = Focused() ? inverted : bold;

  Children children;
  children.push_back(text(L"["));
  if (activated) {
    children.push_back(highlight(text(on)));
    children.push_back(text(L"|"));
    children.push_back(dim(text(off)));
  } else {
    children.push_back(dim(text(on)));
    children.push_back(text(L"|"));
    children.push_back(highlight(text(off)));
  }
  children.push_back(text(L"]"));
  return hbox(std::move(children));
}

bool Toggle::OnEvent(Event event) {
  if (activated) {
    if (event == Event::ArrowRight || event == Event::Character('l')) {
      activated = false;
      on_change();
      return true;
    }
  } else {
    if (event == Event::ArrowLeft || event == Event::Character('h')) {
      activated = true;
      on_change();
      return true;
    }
  }

  return false;
}

}  // namespace component
}  // namespace ftxui
