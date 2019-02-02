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
      std::string code = "";
      for (size_t j = 0; j < 5; ++j)
        code += " " + std::to_string(keys[i].values[j]);

      try {
        std::string line = code + " -> " + std::to_string(keys[i].values[0]) +
                           " (" + char(keys[i].values[0]) + ")";
        children.push_back(text(to_wstring(line)));
      } catch (...) {
        std::string line =
            code + " -> " + std::to_string(keys[i].values[0]) + " (undefined)";
        children.push_back(text(to_wstring(line)));
      }
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
