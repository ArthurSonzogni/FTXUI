// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_LOOP_HPP
#define FTXUI_COMPONENT_LOOP_HPP

#include <memory>  // for shared_ptr

#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/util/export.hpp"               // for FTXUI_EXPORT

namespace ftxui {
class ComponentBase;

using Component = std::shared_ptr<ComponentBase>;
class App;

/// @brief Loop is a class that manages the event loop for a component.
///
/// It is responsible for running the component, handling events, and
/// updating the screen.
///
/// The Loop class is designed to be used with an App object,
/// which represents the terminal screen.
///
/// **Example**
/// ```cpp
/// #include <ftxui/component/component.hpp>
/// #include <ftxui/component/app.hpp>
/// #include <ftxui/component/loop.hpp>
///
/// int main() {
///  auto screen = ftxui::App::TerminalOutput();
///  auto component = ftxui::Button("Click me", [] { ... });
///
///  ftxui::Loop loop(screen.get(), component);
///
///  // Either
///  loop.Run();  // Blocking until the component quits.
///
///  // Or
///  loop.RunOnce();  // Non-blocking, returns immediately.
///
///  // Or
///  loop.RunOnceBlocking();  // Blocking until handling one event.
///
///  // Or in a loop:
///  while (!loop.HasQuitted()) {
///    loop.RunOnce();
///
///    // Do something else like running a different library loop function.
///  }
/// }
/// ```
///
/// @ingroup component
class FTXUI_EXPORT(COMPONENT) Loop {
 public:
  /// Create a manual event loop driving `component` on `screen`.
  Loop(App* screen, Component component);
  ~Loop();

  /// Return true once the loop has been requested to exit.
  bool HasQuitted();
  /// Process pending events and render once without blocking.
  void RunOnce();
  /// Block until at least one event arrives, process it, and render.
  void RunOnceBlocking();
  /// Run the loop until HasQuitted() becomes true.
  void Run();

  // This class is non copyable/movable.
  Loop& operator=(Loop&&) = delete;
  Loop& operator=(const Loop&) = delete;
  Loop(Loop&&) = delete;
  Loop(const Loop&) = delete;

 private:
  App* screen_;
  Component component_;
};

}  // namespace ftxui

#endif  // FTXUI_COMPONENT_LOOP_HPP
