#include <memory>  // for unique_ptr, make_unique, __shared_ptr_access
#include <string>  // for operator+, wstring
#include <vector>  // for vector

#include "ftxui/component/component.hpp"       // for Checkbox, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, size, vbox, border, frame, Elements, HEIGHT, LESS_THAN
#include "ftxui/screen/string.hpp"  // for to_wstring

using namespace ftxui;

struct CheckboxState {
  std::wstring label;
  bool checked;
};

int main(int argc, const char* argv[]) {
  int size = 30;
  std::vector<CheckboxState> states(size);
  auto container = Container::Vertical({});
  for(int i = 0; i<size; ++i) {
    states[i].checked = false;
    states[i].label = L"Checkbox " + to_wstring(i);
    container->Add(Checkbox(&states[i].label, &states[i].checked));
  }

  auto component = Renderer(container, [&] {
    return container->Render() | frame | ftxui::size(HEIGHT, LESS_THAN, 10) |
           border;
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(component);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
