#include <iostream>  // for basic_ostream::operator<<, operator<<, endl, basic_ostream, basic_ostream<>::__ostream_type, cout, ostream
#include <memory>    // for shared_ptr, __shared_ptr_access
#include <string>    // for to_string, allocator

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for MenuEntry, Renderer, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for MenuEntryOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for operator|, separator, Element, Decorator, color, text, hbox, size, bold, frame, inverted, vbox, HEIGHT, LESS_THAN, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Cyan, Color::Green, Color::Red, Color::Yellow

using namespace ftxui;

// Take a list of component, display them vertically, one column shifted to the
// right.
Component Inner(std::vector<Component> children) {
  Component vlist = Container::Vertical(std::move(children));
  return Renderer(vlist, [vlist] {
    return hbox({
        text(" "),
        vlist->Render(),
    });
  });
}

Component Empty() {
  return std::make_shared<ComponentBase>();
}

int main(int argc, const char* argv[]) {
  auto component =
      Collapsible("Collapsible 1",
                  Inner({
                      Collapsible("Collapsible 1.1",
                                  Inner({
                                      Collapsible("Collapsible 1.1.1", Empty()),
                                      Collapsible("Collapsible 1.1.2", Empty()),
                                      Collapsible("Collapsible 1.1.3", Empty()),
                                  })),
                      Collapsible("Collapsible 1.2",
                                  Inner({
                                      Collapsible("Collapsible 1.2.1", Empty()),
                                      Collapsible("Collapsible 1.2.2", Empty()),
                                      Collapsible("Collapsible 1.2.3", Empty()),
                                  })),
                      Collapsible("Collapsible 1.3",
                                  Inner({
                                      Collapsible("Collapsible 1.3.1", Empty()),
                                      Collapsible("Collapsible 1.3.2", Empty()),
                                      Collapsible("Collapsible 1.3.3", Empty()),
                                  })),
                  }));

  ScreenInteractive::FitComponent().Loop(component);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
