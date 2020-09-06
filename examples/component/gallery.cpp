#include "ftxui/component/button.hpp"
#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"

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

    button.label = L"Quit";
    button.on_click = [&] { on_quit(); };
    container.Add(&button);
  }

  Element Render(std::wstring name, Component& component) {
    return hbox({
        text(name) | size(WIDTH, EQUAL, 8),
        separator(),
        component.Render(),
    });
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
            Render(L"input", input) | size(WIDTH, LESS_THAN, 30),
            separator(),
            Render(L"button", button),
        }) |
        border;
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
