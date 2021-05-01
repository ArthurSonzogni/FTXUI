#include <functional>  // for function
#include <memory>      // for allocator, unique_ptr
#include <string>      // for wstring
#include <vector>      // for vector

#include "ftxui/component/button.hpp"              // for Button
#include "ftxui/component/checkbox.hpp"            // for CheckBox
#include "ftxui/component/component.hpp"           // for Component, Compone...
#include "ftxui/component/container.hpp"           // for Container
#include "ftxui/component/input.hpp"               // for Input
#include "ftxui/component/menu.hpp"                // for Menu
#include "ftxui/component/radiobox.hpp"            // for RadioBox
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/component/slider.hpp"              // for Slider
#include "ftxui/component/toggle.hpp"              // for Toggle
#include "ftxui/dom/elements.hpp"                  // for separator, operator|
#include "ftxui/screen/box.hpp"                    // for ftxui

using namespace ftxui;

class MyComponent : public Component {
  Container container = Container::Vertical();
  Menu menu;
  Toggle toggle;
  Container checkbox_container = Container::Vertical();
  CheckBox checkbox1;
  CheckBox checkbox2;
  RadioBox radiobox;
  Input input;
  Button button;

  int slider_value_1_ = 12;
  int slider_value_2_ = 56;
  int slider_value_3_ = 128;
  ComponentPtr slider_1_ = Slider(L"R:", &slider_value_1_, 0, 256, 1);
  ComponentPtr slider_2_ = Slider(L"G:", &slider_value_2_, 0, 256, 1);
  ComponentPtr slider_3_ = Slider(L"B:", &slider_value_3_, 0, 256, 1);

 public:
  MyComponent() {
    Add(&container);
    menu.entries = {
        L"Menu 1",
        L"Menu 2",
        L"Menu 3",
        L"Menu 4",
    };
    container.Add(&menu);

    toggle.entries = {
        L"Toggle_1",
        L"Toggle_2",
    };
    container.Add(&toggle);

    container.Add(&checkbox_container);
    checkbox1.label = L"checkbox1";
    checkbox_container.Add(&checkbox1);
    checkbox2.label = L"checkbox2";
    checkbox_container.Add(&checkbox2);

    radiobox.entries = {
        L"Radiobox 1",
        L"Radiobox 2",
        L"Radiobox 3",
        L"Radiobox 4",
    };
    container.Add(&radiobox);

    input.placeholder = L"Input placeholder";
    container.Add(&input);

    container.Add(slider_1_.get());
    container.Add(slider_2_.get());
    container.Add(slider_3_.get());

    button.label = L"Quit";
    button.on_click = [&] { on_quit(); };
    container.Add(&button);
  }

  Element Render(std::wstring name, Element element) {
    return hbox({
               text(name) | size(WIDTH, EQUAL, 8),
               separator(),
               element | xflex,
           }) |
           xflex;
  }

  Element Render(std::wstring name, Component& component) {
    return Render(name, component.Render());
  }

  Element Render() override {
    return  //
        vbox({
            Render(L"menu", menu),
            separator(),
            Render(L"toggle", toggle),
            separator(),
            Render(L"checkbox", checkbox_container),
            separator(),
            Render(L"radiobox", radiobox),
            separator(),
            Render(L"input", input) | size(WIDTH, LESS_THAN, 50),
            separator(),
            Render(L"slider",  //
                   vbox({
                       slider_1_->Render(),
                       slider_2_->Render(),
                       slider_3_->Render(),
                   })),
            separator(),
            Render(L"button", button),
        }) |
        xflex | size(WIDTH, GREATER_THAN, 40) | border;
  }

  std::function<void()> on_quit = [] {};
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();
  MyComponent component;
  component.on_quit = screen.ExitLoopClosure();
  screen.Loop(&component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
