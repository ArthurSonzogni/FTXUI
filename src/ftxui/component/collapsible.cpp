#include <memory>  // for make_unique, __shared_ptr_access, __shared_ptr_access<>::element_type, shared_ptr
#include <utility>  // for move

#include "ftxui/component/component.hpp"       // for Make, Collapsible
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/dom/node.hpp"                  // for Node

namespace ftxui {

/// @brief A collapsible component. It display a checkbox with an arrow. Once
/// activated, the children is displayed.
/// @params label The label of the checkbox.
/// @params child The children to display.
/// @params show Hold the state about whether the children is displayed or not.
/// 
/// ### Example
/// ```cpp
/// auto component = Collapsible("Show details", details);
/// ```
///
/// ### Output
/// ```
/// 
/// ▼ Show details
/// <details component>
/// ```
Component Collapsible(ConstStringRef label,
                      Component child,
                      Ref<bool> show) {
  class Impl : public ComponentBase {
   public:
    Impl(ConstStringRef label, Component child, Ref<bool> show)
        : label_(label), show_(std::move(show)) {
          CheckboxOption opt;
          opt.style_checked = "▼ ";
          opt.style_unchecked = "▶ ";
          Add(Container::Vertical({
              Checkbox(label_, show_.operator->(), opt),
              Maybe(std::move(child), show_.operator->()),
          }));
    }
    ConstStringRef label_;
    Ref<bool> show_;
  };

  return Make<Impl>(label, std::move(child), std::move(show));
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
