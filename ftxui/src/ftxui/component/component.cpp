#include "ftxui/component/component.hpp"
#include "ftxui/component/delegate.hpp"
#include <assert.h>

namespace ftxui {
namespace component {

Component::Component(Delegate* delegate) {
  delegate_ = delegate;
  delegate_->Register(this);
}

Component::~Component() {}

dom::Element Component::Render() {
  using namespace ftxui::dom;
  return text(L"Not implemented component");
}

bool Component::OnEvent(Event event) {
  return false;
}

bool Component::Focused() {
  Delegate* current = delegate_->Root();
  while (current) {
    if (current == delegate_)
      return true;

    Component* active_child = current->component()->GetActiveChild();
    current = active_child ? active_child->delegate_ : nullptr;
  }
  return false;
}

bool Component::Active() {
  Delegate* parent = delegate_->Parent();
  return parent && parent->component()->GetActiveChild() == this;
}

Component* Component::PreviousSibling() {
  Delegate* sibling = delegate_->PreviousSibling();
  return sibling ? sibling->component() : nullptr;
}

Component* Component::NextSibling() {
  Delegate* sibling = delegate_->NextSibling();
  return sibling ? sibling->component() : nullptr;
}

Component* Component::Parent() {
  Delegate* parent_delegate = delegate_->Parent();
  if (!parent_delegate)
    return nullptr;
  return parent_delegate->component();
}

}  // namespace component
}  // namespace ftxui
