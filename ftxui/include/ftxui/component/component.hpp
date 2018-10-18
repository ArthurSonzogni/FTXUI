#ifndef FTXUI_COMPONENT_COMPONENT_HPP
#define FTXUI_COMPONENT_COMPONENT_HPP

#include "ftxui/component/delegate.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/event.hpp"

namespace ftxui {
namespace component {

class Delegate;
class Focus;

class Component {
 public:
  // Constructor/Destructor.
  Component(Delegate* delegate);
  virtual ~Component();

  // Render the component.
  virtual dom::Element Render();

  // Handle an event. By default, it calls this function on each children.
  virtual bool OnEvent(Event even);

  // If this component contains children, this indicates which one is active. It
  // can be none of them.
  // We say an element has the focus if the chain of GetActiveChild() from the
  // root component contains this object.
  virtual Component* GetActiveChild() { return nullptr; }
  bool Active(); // True is this component is an active child.
  bool Focused(); // True if all the ancestors are active childs.

  Component* Parent();
  Component* PreviousSibling();
  Component* NextSibling();
  
 private:
  Delegate* delegate_;
};

}  // namespace component
}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_HPP */
