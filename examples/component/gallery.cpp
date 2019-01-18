#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"

using namespace ftxui;

class MyComponent : public Component {
  Container container = Container::Vertical();
  Menu menu;
  Toggle toggle;
  Container checkbox_container = Container::Vertical();
  CheckBox checkbox1;
  CheckBox checkbox2;
  RadioBox radiobox;
  Input input;

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

    container.Add(&checkbox_container);
    checkbox1.label = L"checkbox1";
    checkbox_container.Add(&checkbox1);
    checkbox2.label = L"checkbox2";
    checkbox_container.Add(&checkbox2);

    radiobox.entries = {
        L"Browse the web",
        L"Meditate",
        L"Sleep",
        L"Eat",
    };
    container.Add(&radiobox);

    input.placeholder = L"Input placeholder";
    container.Add(&input);
  }

  Element Render(std::wstring name, Component& component) {
    return hbox(text(name) | size(10,1), separator(), component.Render());
  }

  Element Render() override {
    return
      vbox(
        Render(L"menu", menu),
        separator(),
        Render(L"toggle", toggle),
        separator(),
        Render(L"checkbox", checkbox_container),
        separator(),
        Render(L"radiobox", radiobox),
        separator(),
        Render(L"input", input)
      ) | frame;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  screen.Loop(&component);

  return 0;
}
