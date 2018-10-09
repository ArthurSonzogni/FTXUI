#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen_interactive.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/util/string.hpp"

class DrawKey : public ftxui::component::Component {
 public:
  DrawKey(ftxui::component::Delegate* delegate)
      : ftxui::component::Component(delegate) {}

  ftxui::dom::Element Render() override {
    using namespace ftxui::dom;
    Children children;
    for (size_t i = std::max(0, (int)keys.size() - 10); i < keys.size(); ++i) {
      try {
        std::string line = std::to_string(i) + " -> " + std::to_string(keys[i]) +
                           " (" + char(keys[i]) + ")";
        children.push_back(text(to_wstring(line)));
      } catch (...) {
        std::string line = std::to_string(i) + " -> " + std::to_string(keys[i]) +
                           " (undefined)";
        children.push_back(text(to_wstring(line)));
      }
    }
    return vbox(std::move(children));
  }

  bool Event(int key) override {
    keys.push_back(key);
    return true;
  }

 private:
  std::vector<int> keys;
};

int main(int argc, const char* argv[]) {
  ftxui::ScreenInteractive screen(80,10);
  DrawKey draw_key(screen.delegate());
  screen.Loop();
}
