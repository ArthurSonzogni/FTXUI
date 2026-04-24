#include <typeinfo>
#include <initializer_list>
#include <functional>              // for function
#include <string>                  // for allocator, string
#include <utility>                 // for move
import ftxui.component;
import ftxui.dom;
import ftxui.screen;
import ftxui.util;

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.


int main() {
  using namespace ftxui;

  InputOption style_1 = InputOption::Default();

  InputOption style_2 = InputOption::Spacious();

  InputOption style_3 = InputOption::Spacious();
  style_3.transform = [](InputState state) {
    state.element |= borderEmpty;

    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= borderDouble;
      state.element |= bgcolor(Color::White);
      state.element |= color(Color::Black);
    } else if (state.hovered) {
      state.element |= borderRounded;
      state.element |= bgcolor(LinearGradient(90, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    } else {
      state.element |= border;
      state.element |= bgcolor(LinearGradient(0, Color::Blue, Color::Red));
      state.element |= color(Color::White);
    }

    return state.element;
  };

  InputOption style_4 = InputOption::Spacious();
  style_4.transform = [](InputState state) {
    state.element = hbox({
        text("Theorem") | center | borderEmpty | bgcolor(Color::Red),
        separatorEmpty(),
        separator() | color(Color::White),
        separatorEmpty(),
        std::move(state.element),
    });

    state.element |= borderEmpty;
    if (state.is_placeholder) {
      state.element |= dim;
    }

    if (state.focused) {
      state.element |= bgcolor(Color::Black);
    } else {
      state.element |= bgcolor(Color::Blue);
    }

    if (state.hovered) {
      state.element |= bgcolor(Color::GrayDark);
    }

    return vbox({state.element, separatorEmpty()});
  };

  auto generateUiFromStyle = [&](InputOption style) {
    auto first_name = new std::string();   // Leaked
    auto middle_name = new std::string();  // Leaked
    auto last_name = new std::string();    // Leaked
    return Container::Vertical({
               Input(first_name, "first name", style),
               Input(middle_name, "middle name", style),
               Input(last_name, "last name", style),
           }) |
           borderEmpty;
  };

  auto ui = Container::Horizontal({
      generateUiFromStyle(style_1),
      generateUiFromStyle(style_2),
      generateUiFromStyle(style_3),
      generateUiFromStyle(style_4),
  });

  auto screen = App::TerminalOutput();
  screen.Loop(ui);
}
