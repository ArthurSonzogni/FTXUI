#ifndef FTXUI_COMPONENT_CONTAINER_HPP
#define FTXUI_COMPONENT_CONTAINER_HPP

#include "ftxui/component/component.hpp"

namespace ftxui {

// A component where focus and events are automatically handled for you.
// List of container:
//
// Please use HorizontalContainer or VerticalContainer.
class Container : public Component {
 public:
  static Container Vertical();
  static Container Horizontal();

  ~Container() override = default;

  // Component override.
  bool OnEvent(Event event) override;
  Component* ActiveChild() override;
 protected:
  // Handlers
  using Handler = bool (Container::*)(Event);
  Handler handler_;
  bool Vertical(Event event);
  bool Horizontal(Event event);

  size_t selected_ = 0;

  Container(Handler);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_CONTAINER_HPP */
