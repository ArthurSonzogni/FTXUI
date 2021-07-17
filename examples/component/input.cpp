#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for char_traits, operator+, wstring, basic_string

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Renderer, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/deprecated.hpp"         // for Input
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/deprecated.hpp"                // for text
#include "ftxui/dom/elements.hpp"  // for hbox, separator, Element, operator|, vbox, border

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  std::wstring first_name;
  std::wstring last_name;
  std::wstring password;

  Component input_first_name = Input(&first_name, "first name");
  Component input_last_name = Input(&last_name, "last name");

  InputOption password_option;
  password_option.password = true;
  Component input_password = Input(&password, "password", password_option);

  auto component = Container::Vertical({
      input_first_name,
      input_last_name,
      input_password,
  });

  auto renderer = Renderer(component, [&] {
    return vbox({
               text(L"Hello " + first_name + L" " + last_name),
               separator(),
               hbox(text(L" First name : "), input_first_name->Render()),
               hbox(text(L" Last name  : "), input_last_name->Render()),
               hbox(text(L" Password   : "), input_password->Render()),
           }) |
           border;
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
