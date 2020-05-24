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
      if (keys[i].is_character())
        code += keys[i].character();
      else
        code += L"(special)";
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
