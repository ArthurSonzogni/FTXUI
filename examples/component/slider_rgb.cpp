#include <functional>  // for function
#include <memory>      // for allocator, __shared_ptr_access
#include <string>      // for operator+, to_wstring, char_traits

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Slider, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/event.hpp"  // for Event, Event::Escape, Event::Return
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, operator|, Element, size, text, vbox, xflex, bgcolor, hbox, GREATER_THAN, WIDTH, border, HEIGHT, LESS_THAN
#include "ftxui/screen/color.hpp"  // for Color

using namespace ftxui;

Element ColorTile(int red, int green, int blue) {
  return text(L"") | size(WIDTH, GREATER_THAN, 14) |
         size(HEIGHT, GREATER_THAN, 7) | bgcolor(Color::RGB(red, green, blue));
}

Element ColorString(int red, int green, int blue) {
  return text(L"RGB = (" +                     //
              std::to_wstring(red) + L"," +    //
              std::to_wstring(green) + L"," +  //
              std::to_wstring(blue) + L")"     //
  );
}

class MyComponent : public ComponentBase {
 private:
  int* red_;
  int* green_;
  int* blue_;
  Component slider_red_ = Slider(L"Red  :", red_, 0, 255, 1);
  Component slider_green_ = Slider(L"Green:", green_, 0, 255, 1);
  Component slider_blue_ = Slider(L"Blue :", blue_, 0, 255, 1);
  std::function<void(void)> quit_;

 public:
  MyComponent(int* red, int* green, int* blue, std::function<void(void)> quit)
      : red_(red), green_(green), blue_(blue), quit_(quit) {
    Add(Container::Vertical({
        slider_red_,
        slider_green_,
        slider_blue_,
    }));
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
    return ComponentBase::OnEvent(event);
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  int red = 128;
  int green = 25;
  int blue = 100;
  screen.Loop(Make<MyComponent>(&red, &green, &blue, screen.ExitLoopClosure()));
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
