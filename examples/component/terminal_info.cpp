// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for string, to_string
#include <vector>  // for vector
#include <utility> // for move

#include "ftxui/component/app.hpp"          // for App
#include "ftxui/component/component.hpp"    // for Renderer, Button, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/dom/elements.hpp"           // for operator|, Element, text, vbox, border, window
#include "ftxui/screen/color.hpp"           // for Color

int main() {
  using namespace ftxui;
  auto screen = App::FitComponent();

  auto button = Button("Quit", screen.ExitLoopClosure());

  auto layout = Container::Vertical({
      button,
  });

  auto renderer = Renderer(layout, [&] {
    auto capabilities = screen.TerminalCapabilityNames();
    Elements capability_elements;
    for (const auto& cap : capabilities) {
      capability_elements.push_back(text(" - " + cap));
    }

    auto content = vbox({
               text("Terminal Name: " + screen.TerminalName()) | bold,
               text("Terminal Version: " + std::to_string(screen.TerminalVersion())),
               text("Emulator Name: " + screen.TerminalEmulatorName()),
               text("Emulator Version: " + screen.TerminalEmulatorVersion()),
               text("Capabilities:"),
               vbox(std::move(capability_elements)),
               separator(),
               text("Press 'q' or click the button to exit."),
               button->Render(),
           });
    return window(text(" Terminal Info "), content);
  });

  auto component = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.Exit();
      return true;
    }
    return false;
  });

  screen.Loop(component);
}
