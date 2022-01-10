#ifndef FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP
#define FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP

#include <atomic>                        // for atomic
#include <ftxui/component/receiver.hpp>  // for Receiver, Sender
#include <functional>                    // for function
#include <memory>                        // for shared_ptr
#include <string>                        // for string
#include <thread>                        // for thread

#include "ftxui/component/captured_mouse.hpp"  // for CapturedMouse
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/screen/screen.hpp"             // for Screen

namespace ftxui {
class ComponentBase;
struct Event;

using Component = std::shared_ptr<ComponentBase>;

class ScreenInteractive : public Screen {
 public:
  using Callback = std::function<void()>;

  static ScreenInteractive FixedSize(int dimx, int dimy);
  static ScreenInteractive Fullscreen();
  static ScreenInteractive FitComponent();
  static ScreenInteractive TerminalOutput();

  void Loop(Component);
  Callback ExitLoopClosure();

  void PostEvent(Event event);
  CapturedMouse CaptureMouse();

  // Decorate a function. The outputted one will execute similarly to the
  // inputted one, but with the currently active screen terminal hooks
  // temporarily uninstalled.
  Callback WithRestoredIO(Callback);

 private:
  void Install();
  void Uninstall();

  void Main(Component component);
  ScreenInteractive* suspended_screen_ = nullptr;

  void Draw(Component component);
  void EventLoop(Component component);

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

  Sender<Event> event_sender_;
  Receiver<Event> event_receiver_;

  std::string set_cursor_position;
  std::string reset_cursor_position;

  std::atomic<bool> quit_ = false;
  std::thread event_listener_;

  int cursor_x_ = 1;
  int cursor_y_ = 1;

  bool mouse_captured = false;
  bool previous_frame_resized_ = false;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_SCREEN_INTERACTIVE_HPP */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
