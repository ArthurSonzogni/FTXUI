// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP
#define FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP

#include <atomic>                        // for atomic
#include <ftxui/component/receiver.hpp>  // for Receiver, Sender
#include <functional>                    // for function
#include <memory>                        // for shared_ptr
#include <string>                        // for string
#include <thread>                        // for thread
#include <variant>                       // for variant

#include "ftxui/component/animation.hpp"       // for TimePoint
#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/task.hpp"            // for Task, Closure
#include "ftxui/dom/selection.hpp"             // for SelectionOption
#include "ftxui/screen/screen.hpp"             // for Screen

namespace ftxui {
class ComponentBase;
class Loop;
struct Event;

using Component = std::shared_ptr<ComponentBase>;
class ScreenInteractivePrivate;

class ScreenInteractive : public Screen {
 public:
  // Constructors:
  static ScreenInteractive FixedSize(int dimx, int dimy);
  static ScreenInteractive Fullscreen();
  static ScreenInteractive FullscreenPrimaryScreen();
  static ScreenInteractive FullscreenAlternateScreen();
  static ScreenInteractive FitComponent();
  static ScreenInteractive TerminalOutput();

  // Options. Must be called before Loop().
  void TrackMouse(bool enable = true);

  // Return the currently active screen, nullptr if none.
  static ScreenInteractive* Active();

  // Start/Stop the main loop.
  void Loop(Component);
  void Exit();
  Closure ExitLoopClosure();

  // Post tasks to be executed by the loop.
  void Post(Task task);
  void PostEvent(Event event);
  void RequestAnimationFrame();

  CapturedMouse CaptureMouse();

  // Decorate a function. The outputted one will execute similarly to the
  // inputted one, but with the currently active screen terminal hooks
  // temporarily uninstalled.
  Closure WithRestoredIO(Closure);

  // FTXUI implements handlers for Ctrl-C and Ctrl-Z. By default, these handlers
  // are executed, even if the component catches the event. This avoid users
  // handling every event to be trapped in the application. However, in some
  // cases, the application may want to handle these events itself. In this
  // case, the application can force FTXUI to not handle these events by calling
  // the following functions with force=true.
  void ForceHandleCtrlC(bool force);
  void ForceHandleCtrlZ(bool force);

  // Selection API.
  std::string GetSelection();
  void SelectionChange(std::function<void()> callback);

 private:
  void ExitNow();

  void Install();
  void Uninstall();

  void PreMain();
  void PostMain();

  bool HasQuitted();
  void RunOnce(Component component);
  void RunOnceBlocking(Component component);

  void HandleTask(Component component, Task& task);
  bool HandleSelection(bool handled, Event event);
  void RefreshSelection();
  void Draw(Component component);
  void ResetCursorPosition();

  void Signal(int signal);

  ScreenInteractive* suspended_screen_ = nullptr;
  enum class Dimension {
    FitComponent,
    Fixed,
    Fullscreen,
    TerminalOutput,
  };
  Dimension dimension_ = Dimension::Fixed;
  bool use_alternative_screen_ = false;
  ScreenInteractive(int dimx,
                    int dimy,
                    Dimension dimension,
                    bool use_alternative_screen);

  bool track_mouse_ = true;

  Sender<Task> task_sender_;
  Receiver<Task> task_receiver_;

  std::string set_cursor_position;
  std::string reset_cursor_position;

  std::atomic<bool> quit_{false};
  std::thread event_listener_;
  std::thread animation_listener_;
  bool animation_requested_ = false;
  animation::TimePoint previous_animation_time_;

  int cursor_x_ = 1;
  int cursor_y_ = 1;

  bool mouse_captured = false;
  bool previous_frame_resized_ = false;

  bool frame_valid_ = false;

  bool force_handle_ctrl_c_ = true;
  bool force_handle_ctrl_z_ = true;

  // The style of the cursor to restore on exit.
  int cursor_reset_shape_ = 1;

  // Selection API:
  CapturedMouse selection_pending_;
  struct SelectionData {
    int start_x = -1;
    int start_y = -1;
    int end_x = -2;
    int end_y = -2;
    bool empty = true;
    bool operator==(const SelectionData& other) const;
    bool operator!=(const SelectionData& other) const;
  };
  SelectionData selection_data_;
  SelectionData selection_data_previous_;
  std::unique_ptr<Selection> selection_;
  std::function<void()> selection_on_change_;

  friend class Loop;

 public:
  class Private {
   public:
    static void Signal(ScreenInteractive& s, int signal) { s.Signal(signal); }
  };
  friend Private;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP */
