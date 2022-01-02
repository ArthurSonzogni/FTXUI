#include <memory>  // for make_unique, __shared_ptr_access, __shared_ptr_access<>::element_type, shared_ptr
#include <utility>  // for move

#include "ftxui/component/component.hpp"       // for Make, Maybe
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/dom/node.hpp"                  // for Node

namespace ftxui {

Component Maybe(Component child, const bool* show) {
  class Impl : public ComponentBase {
   public:
    Impl(const bool* show) : show_(show) {}

   private:
    Element Render() override {
      return *show_ ? ComponentBase::Render() : std::make_unique<Node>();
    }
    bool Focusable() const override {
      return *show_ && ComponentBase::Focusable();
    }
    bool OnEvent(Event event) override {
      return *show_ && ComponentBase::OnEvent(event);
    }

    const bool* show_;
  };

  auto maybe = Make<Impl>(show);
  maybe->Add(std::move(child));
  return maybe;
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
