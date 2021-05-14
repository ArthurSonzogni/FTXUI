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

/// @brief Return a component, using |render| to render its interface.
/// @param render The function drawing the interface.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// auto renderer = Renderer([] {
///   return text(L"My interface");
/// });
/// screen.Loop(renderer);
/// ```
Component Renderer(std::function<Element()> render) {
  return Make<RendererBase>(std::move(render));
}

/// @brief Return a new Component, similar to |child|, but using |render| as the
/// Component::Render() event.
/// @param child The component to forward events to.
/// @param render The function drawing the interface.
/// @ingroup component
///
/// ### Example
///
/// ```cpp
/// auto screen = ScreenInteractive::TerminalOutput();
/// std::wstring label = "Click to quit";
/// auto button = Button(&label, screen.ExitLoopClosure());
/// auto renderer = Renderer(button, [&] {
///   return hbox({
///     text("A button:"),
///     button->Render(),
///   });
/// });
/// screen.Loop(renderer);
/// ```
Component Renderer(Component child, std::function<Element()> render) {
  Component renderer = Renderer(std::move(render));
  renderer->Add(std::move(child));
  return renderer;
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
