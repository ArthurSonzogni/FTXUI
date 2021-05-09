#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Checkbox, Make
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive, Component

using namespace ftxui;

class MyComponent : public ComponentBase {
 private:
  std::wstring build_examples_label = L"Build examples";
  std::wstring build_tests_label = L"Build tests";
  std::wstring use_webassembly_label = L"Use WebAssembly";

  bool build_examples_state = false;
  bool build_tests_state = false;
  bool use_webassembly_state = true;

  Component container = Container::Vertical({
      Checkbox(&build_examples_label, &build_examples_state),
      Checkbox(&build_tests_label, &build_tests_state),
      Checkbox(&use_webassembly_label, &use_webassembly_state),
  });

 public:
  MyComponent() { Add(container); }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(Make<MyComponent>());
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
