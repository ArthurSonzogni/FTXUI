// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <functional>  // for function
#include <memory>  // for make_unique, __shared_ptr_access, __shared_ptr_access<>::element_type, shared_ptr
#include <utility>  // for move

#include "ftxui/component/component.hpp"  // for ComponentDecorator, Maybe, Make
#include "ftxui/component/component_base.hpp"  // for Component, ComponentBase
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/dom/elements.hpp"              // for Element
#include "ftxui/dom/node.hpp"                  // for Node

namespace ftxui {

/// @brief Decorate a component |child|. It is shown only when |show| returns
/// true.
/// @param child the compoenent to decorate.
/// @param show a function returning whether |child| should shown.
/// @ingroup component
Component Maybe(Component child, std::function<bool()> show) {
  class Impl : public ComponentBase {
   public:
    explicit Impl(std::function<bool()> show) : show_(std::move(show)) {}

   private:
    Element Render() override {
      return show_() ? ComponentBase::Render() : std::make_unique<Node>();
    }
    bool Focusable() const override {
      return show_() && ComponentBase::Focusable();
    }
    bool OnEvent(Event event) override {
      return show_() && ComponentBase::OnEvent(event);
    }

    std::function<bool()> show_;
  };

  auto maybe = Make<Impl>(std::move(show));
  maybe->Add(std::move(child));
  return maybe;
}

/// @brief Decorate a component. It is shown only when the |show| function
/// returns true.
/// @param show a function returning whether the decorated component should
/// be shown.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto component = Renderer([]{ return text("Hello World!"); });
/// auto maybe_component = component | Maybe([&]{ return counter == 42; });
/// ```
ComponentDecorator Maybe(std::function<bool()> show) {
  return [show = std::move(show)](Component child) mutable {
    return Maybe(std::move(child), std::move(show));
  };
}

/// @brief Decorate a component |child|. It is shown only when |show| is true.
/// @param child the compoennt to decorate.
/// @param show a boolean. |child| is shown when |show| is true.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto component = Renderer([]{ return text("Hello World!"); });
/// auto maybe_component = Maybe(component, &show);
/// ```
Component Maybe(Component child, const bool* show) {
  return Maybe(std::move(child), [show] { return *show; });
}

/// @brief Decorate a component. It is shown only when |show| is true.
/// @param show a boolean. |child| is shown when |show| is true.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto component = Renderer([]{ return text("Hello World!"); });
/// auto maybe_component = component | Maybe(&show);
/// ```
ComponentDecorator Maybe(const bool* show) {
  return [show](Component child) { return Maybe(std::move(child), show); };
}

}  // namespace ftxui
