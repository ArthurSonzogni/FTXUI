// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for basic_string, string, allocator
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Dropdown, Horizontal, Vertical
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main() {
  using namespace ftxui;

  std::vector<std::string> entries = {
      "tribute",     "clearance", "ally",        "bend",        "electronics",
      "module",      "era",       "cultural",    "sniff",       "nationalism",
      "negotiation", "deliver",   "figure",      "east",        "tribute",
      "clearance",   "ally",      "bend",        "electronics", "module",
      "era",         "cultural",  "sniff",       "nationalism", "negotiation",
      "deliver",     "figure",    "east",        "tribute",     "clearance",
      "ally",        "bend",      "electronics", "module",      "era",
      "cultural",    "sniff",     "nationalism", "negotiation", "deliver",
      "figure",      "east",
  };

  auto dropdown_1 = Dropdown({
      .radiobox = {.entries = &entries},
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 10),
                  filler(),
              });
            }
            return vbox({
                checkbox,
                filler(),
            });
          },
  });

  auto dropdown_2 = Dropdown({
      .radiobox = {.entries = &entries},
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 10) | bgcolor(Color::Blue),
                  filler(),
              });
            }
            return vbox({
                checkbox | bgcolor(Color::Blue),
                filler(),
            });
          },
  });

  auto dropdown_3 = Dropdown({
      .radiobox =
          {
              .entries = &entries,
              .transform =
                  [](const EntryState& s) {
                    auto t = text(s.label) | borderEmpty;
                    if (s.active) {
                      t |= bold;
                    }
                    if (s.focused) {
                      t |= inverted;
                    }
                    return t;
                  },
          },
      .transform =
          [](bool open, Element checkbox, Element radiobox) {
            checkbox |= borderEmpty;
            if (open) {
              return vbox({
                  checkbox | inverted,
                  radiobox | vscroll_indicator | frame |
                      size(HEIGHT, LESS_THAN, 20) | bgcolor(Color::Red),
                  filler(),
              });
            }
            return vbox({
                checkbox | bgcolor(Color::Red),
                filler(),
            });
          },
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(Container::Horizontal({
      dropdown_1,
      dropdown_2,
      dropdown_3,
  }));
}
