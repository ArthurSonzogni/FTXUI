// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for char_traits, operator+, string, basic_string

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp"  // for Ref


int main() {
  using namespace ftxui;

  // The data:
  std::string first_name;
  std::string last_name;
  std::string password;
  std::string phoneNumber;

  int select_startx = -1;
  int select_starty = -1;
  int select_endx = -1;
  int select_endy = -1;
  std::string textToCopy;

  auto screen = ScreenInteractive::TerminalOutput();

  // The basic input components:
  Component input_first_name = Input(&first_name, "first name");
  Component input_last_name = Input(&last_name, "last name");

  // The password input component:
  InputOption password_option;
  password_option.password = true;
  Component input_password = Input(&password, "password", password_option);

  // The phone number input component:
  // We are using `CatchEvent` to filter out non-digit characters.
  Component input_phone_number = Input(&phoneNumber, "phone number");
  input_phone_number |= CatchEvent([&](Event event) {
    return event.is_character() && !std::isdigit(event.character()[0]);
  });
  input_phone_number |= CatchEvent([&](Event event) {
    return event.is_character() && phoneNumber.size() > 10;
  });

  // The component tree:
  auto component = Container::Vertical({
      input_first_name,
      input_last_name,
      input_password,
      input_phone_number,
  });

  // Tweak how the component tree is rendered:
  auto renderer = Renderer(component, [&] {
    return vbox({
               hbox(text(" First name : "), input_first_name->Render()),
               hbox(text(" Last name  : "), input_last_name->Render()),
               hbox(text(" Password   : "), input_password->Render()),
               hbox(text(" Phone num  : "), input_phone_number->Render()),
               separator(),
               text("Hello " + first_name + " " + last_name),
               text("Your password is " + password),
               text("Your phone number is " + phoneNumber),
               text("select_start " + std::to_string(select_startx) + ";" + std::to_string(select_starty)),
               text("select_end " + std::to_string(select_endx) + ";" + std::to_string(select_endy)),
               text("textToCopy " + textToCopy)
           }) |
           border | selected(select_startx, select_endx);
  });


  renderer |= CatchEvent([&](Event event) { 
    if (event.is_mouse()) {
      auto& mouse = event.mouse();
      if (mouse.button == Mouse::Left) {
        if (mouse.motion == Mouse::Pressed) {
          select_startx = mouse.x;
          select_starty = mouse.y;
          select_endx = mouse.x;
          select_endy = mouse.y;
        } else if (mouse.motion == Mouse::Released) {
          select_endx = mouse.x;
          select_endy = mouse.y;
        }
        else if (mouse.motion == Mouse::Moved) {
          select_endx = mouse.x;
          select_endy = mouse.y;
        }

        screen.PostEvent(Event::Custom);
        return true;
      }
    }

    // if (event == Event::SpecialKey("Ctrl+Shift+C")) {
    //   textToCopy = "Kikoo!";
    //   //clip::set_text(text_to_copy);  // Set the clipboard content

    //   screen.PostEvent(Event::Custom);

    //   return true;
    // }

    return false;    
  });

  screen.Loop(renderer);
}
