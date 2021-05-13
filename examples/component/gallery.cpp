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

// Display a component nicely with a title on the left.
Component Wrap(std::wstring name, Component component) {
  return Renderer(component, [name, component] {
    return hbox({
               text(name) | size(WIDTH, EQUAL, 8),
               separator(),
               component->Render() | xflex,
           }) |
           xflex;
  });
}

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FitComponent();

  // -- Menu ----------------------------------------------------------------------
  const std::vector<std::wstring> menu_entries = {
      L"Menu 1",
      L"Menu 2",
      L"Menu 3",
      L"Menu 4",
  };
  int menu_selected = 0;
  auto menu = Menu(&menu_entries, &menu_selected);
  menu = Wrap(L"Menu", menu);

  // -- Toggle------------------------------------------------------------------
  int toggle_selected = 0;
  std::vector<std::wstring> toggle_entries = {
      L"Toggle_1",
      L"Toggle_2",
  };
  auto toggle = Toggle(&toggle_entries, &toggle_selected);
  toggle = Wrap(L"Toggle", toggle);

  // -- Checkbox ---------------------------------------------------------------
  std::wstring checkbox_1_label = L"checkbox1";
  std::wstring checkbox_2_label = L"checkbox2";
  bool checkbox_1_selected = false;
  bool checkbox_2_selected = false;

  auto checkboxes = Container::Vertical({
      Checkbox(&checkbox_1_label, &checkbox_1_selected),
      Checkbox(&checkbox_2_label, &checkbox_2_selected),
  });
  checkboxes = Wrap(L"Checkbox", checkboxes);

  // -- Radiobox ---------------------------------------------------------------
  int radiobox_selected = 0;
  std::vector<std::wstring> radiobox_entries = {
      L"Radiobox 1",
      L"Radiobox 2",
      L"Radiobox 3",
      L"Radiobox 4",
  };
  auto radiobox = Radiobox(&radiobox_entries, &radiobox_selected);
  radiobox = Wrap(L"Radiobox", radiobox);

  // -- Input ------------------------------------------------------------------
  std::wstring input_label;
  std::wstring input_placeholder = L"input";
  auto input = Input(&input_label, &input_placeholder);
  input = Wrap(L"Input", input);

  // -- Button -----------------------------------------------------------------
  std::wstring button_label = L"Quit";
  std::function<void()> on_button_clicked_;
  auto button = Button(&button_label, screen.ExitLoopClosure());
  button = Wrap(L"Button", button);

  // -- Slider -----------------------------------------------------------------
  int slider_value_1 = 12;
  int slider_value_2 = 56;
  int slider_value_3 = 128;
  auto sliders = Container::Vertical({
      Slider(L"R:", &slider_value_1, 0, 256, 1),
      Slider(L"G:", &slider_value_2, 0, 256, 1),
      Slider(L"B:", &slider_value_3, 0, 256, 1),
  });
  sliders = Wrap(L"Slider", sliders);

  // -- Layout -----------------------------------------------------------------
  auto layout = Container::Vertical({
      menu,
      toggle,
      checkboxes,
      radiobox,
      input,
      sliders,
      button,
  });

  auto component = Renderer(layout, [&] {
    return vbox({
               menu->Render(),
               separator(),
               toggle->Render(),
               separator(),
               checkboxes->Render(),
               separator(),
               radiobox->Render(),
               separator(),
               input->Render(),
               separator(),
               sliders->Render(),
               separator(),
               button->Render(),
           }) |
           xflex | size(WIDTH, GREATER_THAN, 40) | border;
  });

  screen.Loop(component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
