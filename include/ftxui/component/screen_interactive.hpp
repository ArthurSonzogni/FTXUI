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
#include "ftxui/component/task.hpp"            // for Closure, Task
#include "ftxui/screen/screen.hpp"             // for Screen

namespace ftxui {
class ComponentBase;
struct Event;

using Component = std::shared_ptr<ComponentBase>;
class ScreenInteractivePrivate;

class ScreenInteractive : public Screen {
 public:
  // Constructors:
  static ScreenInteractive FixedSize(int dimx, int dimy);
  static ScreenInteractive Fullscreen();
  static ScreenInteractive FitComponent();
  static ScreenInteractive TerminalOutput();

  // Return the currently active screen, nullptr if none.
  static ScreenInteractive* Active();

  void Loop(Component);
  Closure ExitLoopClosure();

  void Post(Task task);
  void PostEvent(Event event);
  void RequestAnimationFrame();

  CapturedMouse CaptureMouse();

  // Decorate a function. The outputted one will execute similarly to the
  // inputted one, but with the currently active screen terminal hooks
  // temporarily uninstalled.
  Closure WithRestoredIO(Closure);

 private:
  void Install();
  void Uninstall();

  void Main(Component component);

  void Draw(Component component);
  void SigStop();

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

  Sender<Task> task_sender_;
  Receiver<Task> task_receiver_;

  std::string set_cursor_position;
  std::string reset_cursor_position;

  std::atomic<bool> quit_ = false;
  std::thread event_listener_;
  std::thread animation_listener_;
  bool animation_requested_ = false;
  animation::TimePoint previous_animation_time;

  int cursor_x_ = 1;
  int cursor_y_ = 1;

  bool mouse_captured = false;
  bool previous_frame_resized_ = false;

 public:
  class Private {
   public:
    static void SigStop(ScreenInteractive& s) { return s.SigStop(); }
  };
  friend Private;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
