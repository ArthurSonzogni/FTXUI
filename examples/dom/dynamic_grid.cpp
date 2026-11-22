// Dynamic grid API example
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  Elements elements;

  elements.push_back(text("element") | border);
  elements.push_back(text("element long name") | border);

  for (int i = 0; i < 7; i++) {
    elements.push_back(text("element") | border);
  }

  elements.push_back(text("element name") | border);

  for (int i = 0; i < 4; i++) {
    elements.push_back(text("element") | border);
  }

  auto renderer = Renderer([&] {
    return dynamic_grid(elements);
  });

  screen.Loop(renderer);
}
