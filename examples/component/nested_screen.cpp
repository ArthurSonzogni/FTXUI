// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for operator+, string, char_traits, basic_string

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Button, Vertical, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, text, Element, operator|, vbox, border

using namespace ftxui;

void Nested(std::string path) {
  auto screen = ScreenInteractive::FitComponent();
  auto back_button = Button("Back", screen.ExitLoopClosure());
  auto goto_1 = Button("Goto /1", [path] { Nested(path + "/1"); });
  auto goto_2 = Button("Goto /2", [path] { Nested(path + "/2"); });
  auto goto_3 = Button("Goto /3", [path] { Nested(path + "/3"); });
  auto layout = Container::Vertical({
      back_button,
      goto_1,
      goto_2,
      goto_3,
  });
  auto renderer = Renderer(layout, [&] {
    return vbox({
               text("path: " + path),
               separator(),
               back_button->Render(),
               goto_1->Render(),
               goto_2->Render(),
               goto_3->Render(),
           }) |
           border;
  });
  screen.Loop(renderer);
}

int main() {
  auto screen = ScreenInteractive::FitComponent();
  auto button_quit = Button("Quit", screen.ExitLoopClosure());
  auto button_nested = Button("Nested", [] { Nested(""); });
  screen.Loop(Container::Vertical({
      button_quit,
      button_nested,
  }));
  return 0;
}
