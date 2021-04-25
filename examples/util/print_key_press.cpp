// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>
#include <thread>

using namespace ftxui;

std::wstring Stringify(Event event) {
  std::wstring out;
  for (auto& it : event.input())
    out += L" " + std::to_wstring((unsigned int)it);

  out = L"(" + out + L" ) -> ";
  if (event.is_character()) {
    out += std::wstring(L"character(") + event.character() + L")";
  } else if (event.is_mouse()) {
    out += L"mouse";
    switch (event.mouse().button) {
      case Mouse::Left:
        out += L"_left";
        break;
      case Mouse::Middle:
        out += L"_middle";
        break;
      case Mouse::Right:
        out += L"_right";
        break;
      case Mouse::None:
        out += L"_none";
        break;
      case Mouse::WheelUp:
        out += L"_wheel_up";
        break;
      case Mouse::WheelDown:
        out += L"_wheel_down";
        break;
    }
    switch (event.mouse().motion) {
      case Mouse::Pressed:
        out += L"_pressed";
        break;
      case Mouse::Released:
        out += L"_released";
        break;
    }
    if (event.mouse().control)
      out += L"_control";
    if (event.mouse().shift)
      out += L"_shift";
    if (event.mouse().meta)
      out += L"_meta";

    out += L"(" +  //
            std::to_wstring(event.mouse().x) + L"," +
            std::to_wstring(event.mouse().y) + L")";
  } else {
    out += L"(special)";
  }
  return out;
}

class DrawKey : public Component {
 public:
  ~DrawKey() override = default;

  Element Render() override {
    Elements children;
    for (size_t i = std::max(0, (int)keys.size() - 20); i < keys.size(); ++i) {
      children.push_back(text(Stringify(keys[i])));
    }
    return window(text(L"keys"), vbox(std::move(children)));
  }

  bool OnEvent(Event event) override {
    keys.push_back(event);
    return true;
  }

 private:
  std::vector<Event> keys;
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  DrawKey draw_key;
  screen.Loop(&draw_key);
}
