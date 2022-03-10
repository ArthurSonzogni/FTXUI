#include <memory>  // for make_unique, __shared_ptr_access, __shared_ptr_access<>::element_type, shared_ptr
#include <utility>  // for move

#include "ftxui/component/component.hpp"       // for Make, Maybe
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/dom/node.hpp"                  // for Node

namespace ftxui {

Component Maybe(Component child, std::function<bool()>&& do_show)
{
  class Impl : public ComponentBase {
   public:
    Impl(std::function<bool()>&& do_show) : do_show_(std::move(do_show)) {}

   private:
    Element Render() override {
      return do_show_() ? ComponentBase::Render() : std::make_unique<Node>();
    }
    bool Focusable() const override {
      return do_show_() && ComponentBase::Focusable();
    }
    bool OnEvent(Event event) override {
      return do_show_() && ComponentBase::OnEvent(event);
    }

    std::function<bool()> do_show_;
  };

  auto maybe = Make<Impl>(std::move(do_show));
  maybe->Add(std::move(child));
  return maybe;
}

ComponentDecorator Maybe(std::function<bool()>&& do_show) {
  return [do_show = std::move(do_show)](Component child) mutable {
    return Maybe(child, std::move(do_show));
  };
}

Component Maybe(Component child, const bool* show) {
  return Maybe(child, [show] {
    return *show;
  });
}

ComponentDecorator Maybe(const bool* show) {
  return [show](Component child) {
    return Maybe(child, show);
  };
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
