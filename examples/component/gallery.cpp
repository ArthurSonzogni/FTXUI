#include <functional>  // for function
#include <memory>      // for allocator, __shared_ptr_access
#include <string>      // for wstring, basic_string
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Slider, Checkbox, Button, Input, Make, Menu, Radiobox, Toggle
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, operator|, Element, size, xflex, text, WIDTH, hbox, vbox, EQUAL, LESS_THAN, border, GREATER_THAN

using namespace ftxui;

class MyComponent : public ComponentBase {
  const std::vector<std::wstring> menu_entries_ = {
      L"Menu 1",
      L"Menu 2",
      L"Menu 3",
      L"Menu 4",
  };
  int menu_selected_ = 0;
  Component menu_ = Menu(&menu_entries_, &menu_selected_);

  int toggle_selected_ = 0;
  std::vector<std::wstring> toggle_entries_ = {
      L"Toggle_1",
      L"Toggle_2",
  };
  Component toggle_ = Toggle(&toggle_entries_, &toggle_selected_);

  std::wstring checkbox_1_label_ = L"checkbox1";
  std::wstring checkbox_2_label_ = L"checkbox2";
  bool checkbox_1_selected_ = false;
  bool checkbox_2_selected_ = false;

  Component checkbox_container_ = Container::Vertical({
      Checkbox(&checkbox_1_label_, &checkbox_1_selected_),
      Checkbox(&checkbox_2_label_, &checkbox_2_selected_),
  });

  int radiobox_selected_ = 0;
  std::vector<std::wstring> radiobox_entries_ = {
      L"Radiobox 1",
      L"Radiobox 2",
      L"Radiobox 3",
      L"Radiobox 4",
  };
  Component radiobox_ = Radiobox(&radiobox_entries_, &radiobox_selected_);

  std::wstring input_label_;
  std::wstring input_placeholder_ = L"input";
  Component input_ = Input(&input_label_, &input_placeholder_);

  std::wstring button_label_ = L"Quit";
  std::function<void()> on_button_clicked_;
  Component button_ = Button(&button_label_, [this] { on_button_clicked_(); });

  int slider_value_1_ = 12;
  int slider_value_2_ = 56;
  int slider_value_3_ = 128;
  Component slider_container_ = Container::Vertical({
      Slider(L"R:", &slider_value_1_, 0, 256, 1),
      Slider(L"G:", &slider_value_2_, 0, 256, 1),
      Slider(L"B:", &slider_value_3_, 0, 256, 1),
  });

 public:
  MyComponent(std::function<void(void)> on_quit) : on_quit_(on_quit) {
    Add(Container::Vertical({
        menu_,
        toggle_,
        checkbox_container_,
        radiobox_,
        input_,
        slider_container_,
        button_,
    }));
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
    return Render(name, component->Render());
  }

  Element Render() override {
    return  //
        vbox({
            Render(L"menu", menu_),
            separator(),
            Render(L"toggle", toggle_),
            separator(),
            Render(L"checkbox", checkbox_container_),
            separator(),
            Render(L"radiobox", radiobox_),
            separator(),
            Render(L"input", input_) | size(WIDTH, LESS_THAN, 50),
            separator(),
            Render(L"slider", slider_container_),
            separator(),
            Render(L"button", button_),
        }) |
        xflex | size(WIDTH, GREATER_THAN, 40) | border;
  }

  std::function<void()> on_quit_;
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();
  auto component = Make<MyComponent>(screen.ExitLoopClosure());
  screen.Loop(component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
