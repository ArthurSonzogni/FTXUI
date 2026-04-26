// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_APP_HPP
#define FTXUI_COMPONENT_APP_HPP

#include <atomic>      // for atomic
#include <chrono>      // for steady_clock, time_point
#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <string>      // for string, basic_string, allocator
#include <vector>      // for vector

#include "ftxui/component/animation.hpp"  // for TimePoint
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/task.hpp"   // for Task, Closure
#include "ftxui/screen/screen.hpp"    // for Screen
#include "ftxui/screen/terminal.hpp"  // for Dimensions

namespace ftxui {
class ComponentBase;
using Component = std::shared_ptr<ComponentBase>;
class Event;
class Selection;
class TaskRunner;

/// @brief App is a class that manages the application lifecycle.
/// It is responsible for initializing the terminal, running the main loop,
/// and cleaning up on exit.
///
/// @note This class was previously named ScreenInteractive.
///
/// @ingroup component
class App : public Screen {
 public:
  // Constructors:

  /// @brief Create an App with a fixed size.
  /// @param dimx The width of the app.
  /// @param dimy The height of the app.
  static App FixedSize(int dimx, int dimy);

  /// @brief Create an App taking the full terminal size. This is using the
  /// alternate screen buffer to avoid messing with the terminal content.
  /// @note This is the same as `App::FullscreenAlternateScreen()`
  static App Fullscreen();

  /// @brief Create an App taking the full terminal size. The primary screen
  /// buffer is being used. It means if the terminal is resized, the previous
  /// content might mess up with the terminal content.
  static App FullscreenPrimaryScreen();

  /// @brief Create an App taking the full terminal size. This is using the
  /// alternate screen buffer to avoid messing with the terminal content.
  static App FullscreenAlternateScreen();

  /// @brief Create an App whose width and height match the component being
  /// drawn.
  static App FitComponent();

  /// @brief Create an App whose width match the terminal output width and
  /// the height matches the component being drawn.
  static App TerminalOutput();

  // Destructor.
  ~App() override;

  App(App&&) noexcept;
  App& operator=(App&&) noexcept;
  App(const App&) = delete;
  App& operator=(const App&) = delete;

  // Options. Must be called before Loop().

  /// @brief Set whether mouse is tracked and events reported.
  /// @param enable Whether to enable mouse event tracking.
  /// @note Mouse tracking is enabled by default.
  /// @note Mouse tracking is only supported on terminals that supports it.
  /// @note This must be called before calling `App::Loop`.
  void TrackMouse(bool enable = true);

  /// @brief Enable or disable automatic piped input handling.
  /// When enabled, FTXUI will detect piped input and redirect stdin from
  /// /dev/tty for keyboard input, allowing applications to read piped data
  /// while still receiving interactive keyboard events.
  /// @param enable Whether to enable piped input handling. Default is true.
  /// @note This must be called before Loop().
  /// @note This feature is enabled by default.
  /// @note This feature is only available on POSIX systems (Linux/macOS).
  void HandlePipedInput(bool enable = true);

  /// @brief Return the currently active app, nullptr if none.
  static App* Active();

  // Start/Stop the main loop.

  /// @brief Execute the main loop.
  /// @param component The component to draw.
  void Loop(Component component);

  /// @brief Exit the main loop.
  void Exit();

  /// @brief Return a function to exit the main loop.
  Closure ExitLoopClosure();

  /// @brief Decorate a function. The outputted one will execute similarly to
  /// the inputted one, but with the currently active app terminal hooks
  /// temporarily uninstalled.
  Closure WithRestoredIO(Closure fn);

  /// @brief FTXUI implements handlers for Ctrl-C and Ctrl-Z. By default, these
  /// handlers are executed, even if the component catches the event. This avoid
  /// users handling every event to be trapped in the application. However, in
  /// some cases, the application may want to handle these events itself. In
  /// this case, the application can force FTXUI to not handle these events by
  /// calling the following functions with force=true.
  void ForceHandleCtrlC(bool force = true);

  /// @brief Force FTXUI to handle or not handle Ctrl-Z, even if the component
  /// catches the Event::CtrlZ.
  void ForceHandleCtrlZ(bool force = true);

  // Post tasks to be executed by the loop.

  /// @brief Add a task to the main loop.
  /// It will be executed later, after every other scheduled tasks.
  void Post(Task task);

  /// @brief Add an event to the main loop.
  /// It will be executed later, after every other scheduled events.
  void PostEvent(Event event);

  /// @brief Add a task to the main loop.
  /// It will be executed later, after every other scheduled tasks.
  static void PostEventOrExecute(Closure closure);

  /// @brief Add a task to draw the screen one more time, until all the
  /// animations are done.
  void RequestAnimationFrame();

  // Selection API:

  /// @brief Try to get the unique lock about being able to capture the mouse.
  /// @return A unique lock if the mouse is not already captured, otherwise a
  /// null.
  CapturedMouse CaptureMouse();

  /// @brief Returns the content of the current selection.
  std::string GetSelection();

  /// @brief Set a callback that will be called when the selection changes.
  void SelectionChange(std::function<void()> callback);

  // Terminal info.

  /// @brief Return the terminal name.
  const std::string& TerminalName() const;

  /// @brief Return the terminal version.
  int TerminalVersion() const;

  /// @brief Return the terminal emulator name.
  const std::string& TerminalEmulatorName() const;

  /// @brief Return the terminal emulator version.
  const std::string& TerminalEmulatorVersion() const;

  /// @brief Return the terminal capabilities.
  const std::vector<int>& TerminalCapabilities() const;

  /// @brief Return the names of the terminal capabilities.
  std::vector<std::string> TerminalCapabilityNames() const;

 private:
  void ExitNow();
  void Install();
  void Uninstall();

  void PreMain();
  void PostMain();

  /// @brief Return whether the main loop has been quit.
  bool HasQuitted();
  void RunOnce(const Component& component);
  void RunOnceBlocking(Component component);

  void HandleTask(Component component, Task& task);
  bool HandleSelection(bool handled, Event event);
  void Draw(Component component);
  std::string ResetCursorPosition();

  void RequestCursorPosition(bool force = false);

  void TerminalSend(std::string_view);
  void TerminalFlush();

  void InstallPipedInputHandling();
  void InstallTerminalInfo();

  void Signal(int signal);

  size_t FetchTerminalEvents();

  void PostAnimationTask();

  struct Internal;
  explicit App(std::unique_ptr<Internal> internal, int dimx, int dimy);

  std::unique_ptr<Internal> internal_;

  friend class Loop;

 public:
  class Private {
   public:
    static void Signal(App& s, int signal) { s.Signal(signal); }
  };
  friend Private;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_APP_HPP */
