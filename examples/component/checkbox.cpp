#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  Component input_list = Container::Vertical({});
  std::vector<std::string> items(100, "");
  for (int i = 0; i < items.size(); ++i) {
    input_list->Add(Input(&(items[i]), "placeholder " + std::to_string(i)));
  }

  auto renderer = Renderer(input_list, [&] {
    return input_list->Render() | vscroll_indicator | frame | border |
           size(HEIGHT, LESS_THAN, 10);
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}
