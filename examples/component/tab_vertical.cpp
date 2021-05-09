#include <memory>  // for __shared_ptr_access
#include <string>  // for wstring, allocator, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Radiobox, Make, Menu
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, separator, hbox, operator|, border

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::vector<std::wstring> tab_values_ = {
      L"tab_1",
      L"tab_2",
      L"tab_3",
  };
  int tab_selected_ = 0;
  Component tab_toggle_ = Menu(&tab_values_, &tab_selected_);

  std::vector<std::wstring> tab_1_entries_ = {
      L"Forest",
      L"Water",
      L"I don't know",
  };
  int tab_1_selected_ = 0;

  std::vector<std::wstring> tab_2_entries_ = {
      L"Hello",
      L"Hi",
      L"Hay",
  };
  int tab_2_selected_ = 0;

  std::vector<std::wstring> tab_3_entries_ = {
      L"Table",
      L"Nothing",
      L"Is",
      L"Empty",
  };
  int tab_3_selected_ = 0;

  Component tab_container_ =
      Container::Tab(&tab_selected_,
                     {
                         Radiobox(&tab_1_entries_, &tab_1_selected_),
                         Radiobox(&tab_2_entries_, &tab_2_selected_),
                         Radiobox(&tab_3_entries_, &tab_3_selected_),
                     });

  Component container_ = Container::Horizontal({
      tab_toggle_,
      tab_container_,
  });

 public:
  MyComponent() { Add(container_); }

  Element Render() {
    return hbox({
               tab_toggle_->Render(),
               separator(),
               tab_container_->Render(),
           }) |
           border;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(Make<MyComponent>());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
