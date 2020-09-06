#include "ftxui/component/button.hpp"

#include "ftxui/component/component.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

class MyComponent : public Component {
 private:
  std::vector<std::unique_ptr<Button>> buttons_;
  Container container_ = Container::Horizontal();

 public:
  MyComponent() {
    Add(&container_);

    auto button_add = std::make_unique<Button>();
    auto button_remove = std::make_unique<Button>();
    container_.Add(button_add.get());
    container_.Add(button_remove.get());
    button_add->label = L"Add one button";
    button_remove->label = L"Remove last button";

    button_add->on_click = [&] {
      auto extra_button = std::make_unique<Button>();
      extra_button->label = L"extra button";
      container_.Add(extra_button.get());
      buttons_.push_back(std::move(extra_button));
    };

    button_remove->on_click = [&] { buttons_.resize(buttons_.size() - 1); };

    buttons_.push_back(std::move(button_add));
    buttons_.push_back(std::move(button_remove));
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
