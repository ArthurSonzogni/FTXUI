// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Toggle, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, vbox, Element

using namespace ftxui;

int main() {
  std::vector<std::string> toggle_1_entries = {
      "On",
      "Off",
  };
  std::vector<std::string> toggle_2_entries = {
      "Enabled",
      "Disabled",
  };
  std::vector<std::string> toggle_3_entries = {
      "10€",
      "0€",
  };
  std::vector<std::string> toggle_4_entries = {
      "Nothing",
      "One element",
      "Several elements",
  };

  int toggle_1_selected = 0;
  int toggle_2_selected = 0;
  int toggle_3_selected = 0;
  int toggle_4_selected = 0;
  Component toggle_1 = Toggle(&toggle_1_entries, &toggle_1_selected);
  Component toggle_2 = Toggle(&toggle_2_entries, &toggle_2_selected);
  Component toggle_3 = Toggle(&toggle_3_entries, &toggle_3_selected);
  Component toggle_4 = Toggle(&toggle_4_entries, &toggle_4_selected);

  auto container = Container::Vertical({
      toggle_1,
      toggle_2,
      toggle_3,
      toggle_4,
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
        text("Choose your options:"),
        text(""),
        hbox(text(" * Poweroff on startup      : "), toggle_1->Render()),
        hbox(text(" * Out of process           : "), toggle_2->Render()),
        hbox(text(" * Price of the information : "), toggle_3->Render()),
        hbox(text(" * Number of elements       : "), toggle_4->Render()),
    });
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}
