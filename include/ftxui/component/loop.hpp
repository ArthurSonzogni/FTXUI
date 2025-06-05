// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_LOOP_HPP
#define FTXUI_COMPONENT_LOOP_HPP

#include <memory>  // for shared_ptr

#include "ftxui/component/component_base.hpp"  // for ComponentBase

namespace ftxui {
class ComponentBase;

using Component = std::shared_ptr<ComponentBase>;
class ScreenInteractive;

/// @brief Loop is a class that manages the event loop for a component.
///
/// It is responsible for running the component, handling events, and
/// updating the screen.
///
/// The Loop class is designed to be used with a ScreenInteractive object,
/// which represents the terminal screen.
///
/// **Example**
/// ```cpp
/// #include <ftxui/component/component.hpp>
/// #include <ftxui/component/screen_interactive.hpp>
/// #include <ftxui/component/loop.hpp>
///
/// int main() {
///  auto screen = ftxui::ScreenInteractive::TerminalOutput();
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
class Loop {
 public:
  Loop(ScreenInteractive* screen, Component component);
  ~Loop();

  bool HasQuitted();
  void RunOnce();
  void RunOnceBlocking();
  void Run();

  // This class is non copyable/movable.
  Loop(const Loop&) = default;
  Loop(Loop&&) = delete;
  Loop& operator=(Loop&&) = delete;
  Loop(const ScreenInteractive&) = delete;
  Loop& operator=(const Loop&) = delete;

 private:
  ScreenInteractive* screen_;
  Component component_;
};

}  // namespace ftxui

#endif  // FTXUI_COMPONENT_LOOP_HPP
