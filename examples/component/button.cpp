#include <memory>  // for shared_ptr, allocator, __shared_ptr_access
 
#include "ftxui/component/component.hpp"  // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, center, border
 
using namespace ftxui;
 
int main() {
  auto screen = ScreenInteractive::Fullscreen();

  std::string a = "aa";

  auto component = Renderer([a] { 

    std::vector<Element> elements;

    for (auto i = 0; i < 10; ++i) {
      elements.push_back(window(text(a), paragraph(a)));
    }

    return vbox(elements);
  });
 
  auto renderer =
      Renderer(component, [&] { return component->Render() | border; });
 
  screen.Loop(renderer);
}
