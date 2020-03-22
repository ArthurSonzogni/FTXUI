#include "ftxui/component/component.hpp"

#include <assert.h>

#include <algorithm>

namespace ftxui {
void Component::Detach() {
  if (!parent_)
    return;
  auto it = std::find(std::begin(parent_->children_),
                      std::end(parent_->children_), this);
  parent_->children_.erase(it);
}

void Component::Attach(Component* parent) {
  Detach();
  parent_ = parent;
  parent_->children_.push_back(this);
}

void Component::Add(Component* child) {
  child->Attach(this);
}

Component::~Component() {
  Detach();
}

bool Component::OnEvent(Event event) {
  for (Component* child : children_) {
    if (child->OnEvent(event))
      return true;
  }
  return false;
}

Component* Component::ActiveChild() {
  return children_.empty() ? nullptr : children_.front();
}

Element Component::Render() {
  if (children_.size() == 1)
    return children_.front()->Render();

  return text(L"Not implemented component");
}

bool Component::Focused() {
  Component* current = this;
  for (;;) {
    Component* parent = current->parent_;
    if (!parent)
      return true;
    if (parent->ActiveChild() != current)
      return false;
    current = parent;
  }
}

}  // namespace ftxui
