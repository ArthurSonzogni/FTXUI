#include "ftxui/component/slider.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

Element ColorTile(int red, int green, int blue) {
  return text(L"")
    | size(WIDTH, GREATER_THAN, 14)
    | size(HEIGHT, GREATER_THAN, 7)
    | bgcolor(Color::RGB(red, green, blue));
}

Element ColorString(int red, int green, int blue) {
  return text(L"RGB = (" +                     //
              std::to_wstring(red) + L"," +    //
              std::to_wstring(green) + L"," +  //
              std::to_wstring(blue) + L")"     //
  );
}

class MyComponent : public Component {
 public:
  MyComponent(int* red, int* green, int* blue, std::function<void(void)> quit)
      : red_(red), green_(green), blue_(blue), quit_(quit) {
    Add(&container_);
    container_.Add(slider_red_.get());
    container_.Add(slider_green_.get());
    container_.Add(slider_blue_.get());
  }

  Element Render() {
    return hbox({
               ColorTile(*red_, *green_, *blue_),
               separator(),
               vbox({
                   slider_red_->Render(),
                   separator(),
                   slider_green_->Render(),
                   separator(),
                   slider_blue_->Render(),
                   separator(),
                   ColorString(*red_, *green_, *blue_),
               }) | xflex,
           }) |
           border | size(WIDTH, LESS_THAN, 80);
  }

  bool OnEvent(Event event) {
    if (event == Event::Return || event == Event::Escape)
      quit_();
    return Component::OnEvent(event);
  }

 private:
  int* red_;
  int* green_;
  int* blue_;
  Container container_ = Container::Vertical();
  ComponentPtr slider_red_ = Slider(L"Red  :", red_, 0, 255, 1);
  ComponentPtr slider_green_ = Slider(L"Green:", green_, 0, 255, 1);
  ComponentPtr slider_blue_ = Slider(L"Blue :", blue_, 0, 255, 1);
  std::function<void(void)> quit_;
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  int red = 128;
  int green = 25;
  int blue = 100;
  auto component = MyComponent(&red, &green, &blue, screen.ExitLoopClosure());
  screen.Loop(&component);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
