#ifndef FTXUI_COMPONENT_DELEGATE_HPP
#define FTXUI_COMPONENT_DELEGATE_HPP

#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace component {

class Component;

class Delegate {
 public:
  Delegate() {}
  virtual ~Delegate() {}

  // A Delegate shadows a component.
  virtual void Register(Component* component) = 0;
  virtual Component* component() = 0;

  // Create new children.
  virtual Delegate* NewChild() = 0;
  virtual std::vector<Delegate*> children() = 0;

  // Navigate in the tree.
  virtual Delegate* PreviousSibling() = 0;
  virtual Delegate* NextSibling() = 0;
  virtual Delegate* Parent() = 0;
  virtual Delegate* Root() = 0;
};

}  // namespace component
}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_DELEGATE_HPP */
