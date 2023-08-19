// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <ftxui/component/component_options.hpp>  // for ButtonOption
#include <ftxui/component/mouse.hpp>              // for ftxui
#include <functional>                             // for function
#include <memory>                                 // for allocator, shared_ptr

#include "ftxui/component/component.hpp"  // for Button, operator|=, Renderer, Vertical, Modal
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive, Component
#include "ftxui/dom/elements.hpp"  // for operator|, separator, text, size, Element, vbox, border, GREATER_THAN, WIDTH, center, HEIGHT

using namespace ftxui;

auto button_style = ButtonOption::Animated();

// Definition of the main component. The details are not important.
Component MainComponent(std::function<void()> show_modal,
                        std::function<void()> exit) {
  auto component = Container::Vertical({
      Button("Show modal", show_modal, button_style),
      Button("Quit", exit, button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Main component"),
               separator(),
               inner,
           })                                //
           | size(WIDTH, GREATER_THAN, 15)   //
           | size(HEIGHT, GREATER_THAN, 15)  //
           | border                          //
           | center;                         //
  });
  return component;
}

// Definition of the modal component. The details are not important.
Component ModalComponent(std::function<void()> do_nothing,
                         std::function<void()> hide_modal) {
  auto component = Container::Vertical({
      Button("Do nothing", do_nothing, button_style),
      Button("Quit modal", hide_modal, button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Modal component "),
               separator(),
               inner,
           })                               //
           | size(WIDTH, GREATER_THAN, 30)  //
           | border;                        //
  });
  return component;
}

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();

  // State of the application:
  bool modal_shown = false;

  // Some actions modifying the state:
  auto show_modal = [&] { modal_shown = true; };
  auto hide_modal = [&] { modal_shown = false; };
  auto exit = screen.ExitLoopClosure();
  auto do_nothing = [&] {};

  // Instanciate the main and modal components:
  auto main_component = MainComponent(show_modal, exit);
  auto modal_component = ModalComponent(do_nothing, hide_modal);

  // Use the `Modal` function to use together the main component and its modal
  // window. The |modal_shown| boolean controls whether the modal is shown or
  // not.
  main_component |= Modal(modal_component, &modal_shown);

  screen.Loop(main_component);
  return 0;
}
