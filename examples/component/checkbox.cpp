#include "ftxui/component/checkbox.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

class MyComponent : public Component {
 private:
  CheckBox box_1_;
  CheckBox box_2_;
  CheckBox box_3_;
  Container container_ = Container::Vertical();

 public:
  MyComponent() {
    Add(&container_);
    container_.Add(&box_1_);
    container_.Add(&box_2_);
    container_.Add(&box_3_);
    box_1_.label = L"Build examples";
    box_2_.label = L"Build tests";
    box_3_.label = L"Use WebAssembly";
    box_3_.state = true;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  screen.Loop(&component);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
