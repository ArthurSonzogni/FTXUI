// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <array>  // for array
#include <iostream>
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Checkbox, Renderer, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, Element, size, border, frame, vscroll_indicator, HEIGHT, LESS_THAN

using namespace ftxui;

int main() {
  bool download = false;
  bool upload = false;
  bool ping = false;

  auto container = Container::Vertical({
      Checkbox("Download", &download),
      Checkbox("Upload", &upload),
      Checkbox("Ping", &ping),
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(container);

  std::cout << "---" << std::endl;
  std::cout << "Download: " << download << std::endl;
  std::cout << "Upload: " << upload << std::endl;
  std::cout << "Ping: " << ping << std::endl;
  std::cout << "---" << std::endl;

  return 0;
}
