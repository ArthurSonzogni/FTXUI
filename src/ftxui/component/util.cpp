#include <functional>  // for function
#include <memory>      // for __shared_ptr_access

#include "ftxui/component/component.hpp"  // for ElementDecorator, Renderer, ComponentDecorator, operator|, operator|=
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/dom/elements.hpp"              // for Element

namespace ftxui {

Component operator|(Component component, ComponentDecorator decorator) {
  return decorator(component);
}

Component operator|(Component component, ElementDecorator decorator) {
  return Renderer(component, [component, decorator] {
    return decorator(component->Render());
  });
}

Component& operator|=(Component& component, ComponentDecorator decorator) {
  component = component | decorator;
  return component;
}

Component& operator|=(Component& component, ElementDecorator decorator) {
  component = component | decorator;
  return component;
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
