#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/string.hpp"

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
    return vbox(std::move(children));
  }

  bool OnEvent(Event event) override {
    keys.push_back(event);
    return true;
  }

 private:
  std::vector<Event> keys;
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::FixedSize(80, 10);
  DrawKey draw_key;
  screen.Loop(&draw_key);
}
