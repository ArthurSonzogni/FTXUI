// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>  // for operator""ms, literals
#include <memory>  // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for string, operator+, to_string, basic_string
#include <vector>  // for vector

#include "ftxui/component/animation.hpp"       // for BackOut, Duration
#include "ftxui/component/component.hpp"       // for Menu, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, UnderlineOption
#include "ftxui/component/mouse.hpp"              // for ftxui
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, Element, operator|, borderEmpty, inverted
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Red

using namespace ftxui;

Component DummyComponent(int id) {
  return Renderer([id](bool focused) {
    auto t = text("component " + std::to_string(id));
    if (focused)
      t = t | inverted;
    return t;
  });
}

Component Text(const std::string& t) {
  return Renderer([t] { return text(t) | borderEmpty; });
}

int main() {
  using namespace std::literals;
  std::vector<std::string> tab_values{
      "Tab 1", "Tab 2", "Tab 3", "A very very long tab", "탭",
  };
  int tab_selected = 0;

  auto container = Container::Vertical({});

  int frame_count = 0;
  container->Add(Renderer(
      [&] { return text("Frame count: " + std::to_string(frame_count++)); }));

  {
    auto option = MenuOption::HorizontalAnimated();
    container->Add(Text("This demonstrate the Menu component"));
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  {
    container->Add(Text("Set underline color to blue"));
    auto option = MenuOption::HorizontalAnimated();
    option.underline.color_inactive = Color::Blue;
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  {
    container->Add(Text("Set underline active color to red"));
    auto option = MenuOption::HorizontalAnimated();
    option.underline.color_active = Color::Red;
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  {
    container->Add(Text("Set animation duration to 0ms"));
    auto option = MenuOption::HorizontalAnimated();
    option.underline.SetAnimationDuration(0ms);
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  {
    container->Add(Text("Set animation easing function to back-out"));
    auto option = MenuOption::HorizontalAnimated();
    option.underline.SetAnimationFunction(animation::easing::BackOut);
    option.underline.SetAnimationDuration(350ms);
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  // option.underline_animation_follower_delay = 250ms
  {
    container->Add(Text("Add delay to desynchronize animation"));
    auto option = MenuOption::HorizontalAnimated();
    option.underline.follower_delay = 250ms;
    container->Add(Menu(&tab_values, &tab_selected, option));
  }

  container->SetActiveChild(container->ChildAt(2));

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(container);
}
