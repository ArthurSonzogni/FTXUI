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

class DrawKey : public Component {
 public:
  ~DrawKey() override = default;

  Element Render() override {
    Elements children;
    for (size_t i = std::max(0, (int)keys.size() - 10); i < keys.size(); ++i) {
      std::wstring code;
      for (auto& it : keys[i].input())
        code += L" " + std::to_wstring((unsigned int)it);

      code = L"(" + code + L" ) -> ";
      if (keys[i].is_character()) {
        code += std::wstring(L"character(") + keys[i].character() + L")";
      } else if (keys[i].is_mouse_move()) {
        code += L"mouse_move(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else if (keys[i].is_mouse_up()) {
        code += L"mouse_up(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else if (keys[i].is_mouse_left_down()) {
        code += L"mouse_left_down(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else if (keys[i].is_mouse_left_move()) {
        code += L"mouse_left_move(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else if (keys[i].is_mouse_right_down()) {
        code += L"mouse_right_down(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else if (keys[i].is_mouse_right_move()) {
        code += L"mouse_right_move(" +  //
                std::to_wstring(keys[i].mouse_x()) + L"," +
                std::to_wstring(keys[i].mouse_y()) + L")";
      } else {
        code += L"(special)";
      }
      children.push_back(text(code));
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
