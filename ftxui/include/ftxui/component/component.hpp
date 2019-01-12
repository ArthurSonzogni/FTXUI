#ifndef FTXUI_COMPONENT_COMPONENT_HPP
#define FTXUI_COMPONENT_COMPONENT_HPP

#include "ftxui/component/delegate.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

class Delegate;
class Focus;

class Component {
 public:

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

  // Constructor/Destructor.
  Component(Delegate* delegate);
  virtual ~Component();

  // Render the component.
  virtual Element Render();

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

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_COMPONENT_HPP */
