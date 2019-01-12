#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"

using namespace ftxui;

class MyComponent : public Component {
  Container container = Container::Vertical();
  Menu menu;
  Input input;
  Toggle toggle;

 public:
  MyComponent() {
    Add(&container);
    menu.entries = {
        L"Browse the web",
        L"Meditate",
        L"Sleep",
        L"Eat",
    };
    container.Add(&menu);

    toggle.entries = {
        L"Browse the web",
        L"Meditate",
        L"Sleep",
        L"Eat",
    };
    container.Add(&toggle);

    input.placeholder = L"Input placeholder";
    container.Add(&input);
  }

  Element Render() override {
    return
      vbox(
        hbox(text(L"menu") | size(10,1), separator(), menu.Render()),
        separator(),
        hbox(text(L"toggle") | size(10,1), separator(), toggle.Render()),
        separator(),
        hbox(text(L"input") | size(10,1), separator(), input.Render())
      ) | frame;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  screen.Loop(&component);

  return 0;
}
