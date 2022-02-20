#include <array>       // for array
#include <chrono>      // for milliseconds
#include <functional>  // for function
#include <memory>      // for shared_ptr, __shared_ptr_access, allocator
#include <string>      // for string, char_traits, basic_string, operator+
#include <vector>      // for vector

#include "ftxui/component/animation.hpp"  // for ElasticOut, Linear
#include "ftxui/component/component.hpp"  // for Menu, Horizontal, Renderer, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption, MenuEntryOption, AnimatedColorOption, AnimatedColorsOption, UnderlineOption
#include "ftxui/component/mouse.hpp"              // for ftxui
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, operator|, Element, text, bgcolor, hbox, bold, color, filler, border, vbox, borderDouble, dim, flex, hcenter
#include "ftxui/screen/color.hpp"  // for Color, Color::Red, Color::Black, Color::Yellow, Color::Blue, Color::Default, Color::White

using namespace ftxui;

Component VMenu1(std::vector<std::string>* entries, int* selected);
Component VMenu2(std::vector<std::string>* entries, int* selected);
Component VMenu3(std::vector<std::string>* entries, int* selected);
Component VMenu4(std::vector<std::string>* entries, int* selected);
Component VMenu5(std::vector<std::string>* entries, int* selected);
Component VMenu6(std::vector<std::string>* entries, int* selected);
Component VMenu7(std::vector<std::string>* entries, int* selected);
Component VMenu8(std::vector<std::string>* entries, int* selected);
Component HMenu1(std::vector<std::string>* entries, int* selected);
Component HMenu2(std::vector<std::string>* entries, int* selected);
Component HMenu3(std::vector<std::string>* entries, int* selected);
Component HMenu4(std::vector<std::string>* entries, int* selected);
Component HMenu5(std::vector<std::string>* entries, int* selected);

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<std::string> entries = {
      "Monkey", "Dog", "Cat", "Bird", "Elephant", "Cat",
  };
  std::array<int, 12> selected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto vmenu_1_ = VMenu1(&entries, &selected[0]);
  auto vmenu_2_ = VMenu2(&entries, &selected[1]);
  auto vmenu_3_ = VMenu3(&entries, &selected[2]);
  auto vmenu_4_ = VMenu4(&entries, &selected[3]);
  auto vmenu_5_ = VMenu5(&entries, &selected[4]);
  auto vmenu_6_ = VMenu6(&entries, &selected[5]);
  auto vmenu_7_ = VMenu7(&entries, &selected[6]);
  auto vmenu_8_ = VMenu8(&entries, &selected[7]);

  auto hmenu_1_ = HMenu1(&entries, &selected[8]);
  auto hmenu_2_ = HMenu2(&entries, &selected[9]);
  auto hmenu_3_ = HMenu3(&entries, &selected[10]);
  auto hmenu_4_ = HMenu4(&entries, &selected[11]);
  auto hmenu_5_ = HMenu5(&entries, &selected[12]);

  auto container = Container::Vertical({
      Container::Horizontal({
          vmenu_1_,
          vmenu_2_,
          vmenu_3_,
          vmenu_4_,
          vmenu_5_,
          vmenu_6_,
          vmenu_7_,
          vmenu_8_,
      }),
      hmenu_1_,
      hmenu_2_,
      hmenu_3_,
      hmenu_4_,
      hmenu_5_,
  });

  auto renderer = Renderer(container, [&] {
    return  //
        hbox({
            vbox({
                hbox({
                    vmenu_1_->Render(),
                    separator(),
                    vmenu_2_->Render(),
                    separator(),
                    vmenu_3_->Render(),
                    separator(),
                    vmenu_4_->Render(),
                    separator(),
                    vmenu_5_->Render(),
                    vmenu_6_->Render(),
                    separator(),
                    vmenu_7_->Render(),
                    separator(),
                    vmenu_8_->Render(),
                }),
                separator(),
                hmenu_1_->Render(),
                separator(),
                hmenu_2_->Render(),
                separator(),
                hmenu_3_->Render(),
                separator(),
                hmenu_4_->Render(),
                hmenu_5_->Render(),
            }) | border,
            filler(),
        });
  });

  screen.Loop(renderer);
}

Component VMenu1(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    label = (selected ? "> " : "  ") + label;
    Element e = text(label);
    if (focused)
      e = e | bgcolor(Color::Blue);
    if (selected)
      e = e | bold;
    return e;
  };
  return Menu(entries, selected, option);
}

Component VMenu2(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    label = label + (selected ? " <" : "  ");
    Element e = hbox(filler(), text(label));
    if (focused)
      e = e | bgcolor(Color::Red);
    if (selected)
      e = e | bold;
    return e;
  };
  return Menu(entries, selected, option);
}

Component VMenu3(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    Element e = selected ? text("[" + label + "]") : text(" " + label + " ");
    if (focused)
      e = e | bold;

    if (focused)
      e = e | color(Color::Blue);
    if (selected)
      e = e | bold;
    return e;
  };
  return Menu(entries, selected, option);
}

Component VMenu4(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    if (selected && focused)
      return text(label) | color(Color::Yellow) | bgcolor(Color::Black) | bold;
    else if (selected)
      return text(label) | color(Color::Yellow) | bgcolor(Color::Black);
    else if (focused)
      return text(label) | color(Color::Black) | bgcolor(Color::Yellow) | bold;
    else
      return text(label) | color(Color::Black) | bgcolor(Color::Yellow);
  };
  return Menu(entries, selected, option);
}

Component VMenu5(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    if (selected && focused)
      return text(label) | borderDouble;
    else if (selected)
      return text(label) | border;
    else if (focused)
      return text(label) | bold;
    else
      return text(label);
  };
  return Menu(entries, selected, option);
}

Component VMenu6(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::VerticalAnimated();
  option.underline.color_inactive = Color::Default;
  option.underline.color_active = Color::Red;
  option.underline.SetAnimationFunction(animation::easing::Linear);
  return Menu(entries, selected, option);
}

Component VMenu7(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.animated_colors.foreground.enabled = true;
  option.entries.animated_colors.background.enabled = true;
  option.entries.animated_colors.background.active = Color::Red;
  option.entries.animated_colors.background.inactive = Color::Black;
  option.entries.animated_colors.foreground.active = Color::White;
  option.entries.animated_colors.foreground.inactive = Color::Red;
  return Menu(entries, selected, option);
}

Component VMenu8(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Vertical();
  option.entries.animated_colors.foreground.Set(Color::Red, Color::White,
                                                std::chrono::milliseconds(500));
  return Menu(entries, selected, option);
}

Component HMenu1(std::vector<std::string>* entries, int* selected) {
  return Menu(entries, selected, MenuOption::Horizontal());
}

Component HMenu2(std::vector<std::string>* entries, int* selected) {
  return Menu(entries, selected, MenuOption::Toggle());
}

Component HMenu3(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::Toggle();
  option.elements_infix = [] { return text(" 🮣🮠 "); };

  return Menu(entries, selected, option);
}

Component HMenu4(std::vector<std::string>* entries, int* selected) {
  return Menu(entries, selected, MenuOption::HorizontalAnimated());
}

Component HMenu5(std::vector<std::string>* entries, int* selected) {
  auto option = MenuOption::HorizontalAnimated();
  option.underline.SetAnimation(std::chrono::milliseconds(1500),
                                animation::easing::ElasticOut);
  option.entries.transform = [](std::string label, bool focused,
                                bool selected) {
    Element e = text(label) | hcenter | flex;
    if (selected && focused)
      e = e | bold;
    if (!focused && !selected)
      e = e | dim;
    return e;
  };
  option.underline.color_inactive = Color::Default;
  option.underline.color_active = Color::Red;
  return Menu(entries, selected, option);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
