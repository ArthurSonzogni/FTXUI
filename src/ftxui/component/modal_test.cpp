// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <ftxui/dom/elements.hpp>  // for Element, operator|, text, border

#include "ftxui/component/component.hpp"       // for Renderer, Modal
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/dom/node.hpp"                  // for Render
#include "ftxui/screen/screen.hpp"             // for Screen

// NOLINTBEGIN
namespace ftxui {

TEST(ModalTest, Basic) {
  auto main = Renderer([] { return text("main") | border; });
  auto modal = Renderer([] { return text("modal") | border; });
  bool show_modal = false;
  auto component = Modal(main, modal, &show_modal);

  Screen screen(10, 7);
  Render(screen, component->Render());
  EXPECT_EQ(screen.ToString(),
            "╭────────╮\r\n"
            "│main    │\r\n"
            "│        │\r\n"
            "│        │\r\n"
            "│        │\r\n"
            "│        │\r\n"
            "╰────────╯");

  show_modal = true;
  Render(screen, component->Render());
  EXPECT_EQ(screen.ToString(),
            "╭────────╮\r\n"
            "│main    │\r\n"
            "│╭─────╮ │\r\n"
            "││modal│ │\r\n"
            "│╰─────╯ │\r\n"
            "│        │\r\n"
            "╰────────╯");
}

}  // namespace ftxui
// NOLINTEND
