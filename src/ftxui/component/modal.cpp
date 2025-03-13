// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/event.hpp>  // for Event
#include <ftxui/dom/elements.hpp>  // for operator|, Element, center, clear_under, dbox
#include <memory>                  // for __shared_ptr_access, shared_ptr
#include <utility>                 // for move

#include "ftxui/component/component.hpp"  // for Make, Tab, ComponentDecorator, Modal
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase

namespace ftxui {

// Add a |modal| window on top of the |main| component. It is shown one on the
// top of the other when |show_modal| is true.
/// @ingroup component
// NOLINTNEXTLINE
Component Modal(Component main, Component modal, const bool* show_modal) {
  class Impl : public ComponentBase {
   public:
    explicit Impl(Component main, Component modal, const bool* show_modal)
        : main_(std::move(main)),
          modal_(std::move(modal)),
          show_modal_(show_modal) {
      Add(Container::Tab({main_, modal_}, &selector_));
    }

   private:
    Element OnRender() override {
      selector_ = *show_modal_;
      auto document = main_->Render();
      if (*show_modal_) {
        document = dbox({
            document,
            modal_->Render() | clear_under | center,
        });
      }
      return document;
    }

    bool OnEvent(Event event) override {
      selector_ = *show_modal_;
      return ComponentBase::OnEvent(event);
    }

    Component main_;
    Component modal_;
    const bool* show_modal_;
    int selector_ = *show_modal_;
  };
  return Make<Impl>(main, modal, show_modal);
}

// Decorate a component. Add a |modal| window on top of it. It is shown one on
// the top of the other when |show_modal| is true.
/// @ingroup component
// NOLINTNEXTLINE
ComponentDecorator Modal(Component modal, const bool* show_modal) {
  return [modal, show_modal](Component main) {
    return Modal(std::move(main), modal, show_modal);
  };
}

}  // namespace ftxui
