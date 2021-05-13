#include <functional>  // for function
#include <memory>      // for shared_ptr

#include "ftxui/component/component.hpp"       // for Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/elements.hpp"              // for Element

namespace ftxui {

/// @brief A component rendering Element from a function.
/// @ingroup component.
class RendererBase : public ComponentBase {
 public:
  // Access this interface from a Component
  static RendererBase* From(Component component) {
    return static_cast<RendererBase*>(component.get());
  }

  // Constructor.
  RendererBase(std::function<Element()> render) : render_(std::move(render)) {}
  ~RendererBase() override = default;

  // Component implementation.
  Element Render() override { return render_(); }

 protected:
  std::function<Element()> render_;
};

Component Renderer(std::function<Element()> render) {
  return Make<RendererBase>(std::move(render));
}

Component Renderer(Component child, std::function<Element()> render) {
  Component renderer = Renderer(std::move(render));
  renderer->Add(std::move(child));
  return renderer;
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
