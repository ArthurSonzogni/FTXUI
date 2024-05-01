#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Collapsible, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown
#include "ftxui/dom/elements.hpp"     // for Element, text
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, Test, TestPartResult, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

TEST(CollapsibleTest, Basic) {
  auto child = Renderer([] { return text("child"); });
  bool show = false;
  auto collapsible = Collapsible("parent", child, &show);

  EXPECT_TRUE(collapsible->Focused());
  EXPECT_FALSE(child->Focused());
  EXPECT_FALSE(collapsible->OnEvent(Event::ArrowDown));
  EXPECT_TRUE(collapsible->Focused());
  EXPECT_FALSE(child->Focused());

  {
    Screen screen(8, 3);
    Render(screen, collapsible->Render());
    EXPECT_EQ(screen.ToString(),
              "\xE2\x96\xB6 \x1B[1m\x1B[7mparent\x1B[22m\x1B[27m\r\n"
              "        \r\n"
              "        ");
  }

  collapsible->OnEvent(Event::Return);
  EXPECT_EQ(show, true);

  {
    Screen screen(8, 3);
    Render(screen, collapsible->Render());
    EXPECT_EQ(screen.ToString(),
              "\xE2\x96\xBC \x1B[1m\x1B[7mparent\x1B[22m\x1B[27m\r\n"
              "child   \r\n"
              "        ");
  }

  collapsible->OnEvent(Event::Return);
  EXPECT_EQ(show, false);
}

}  // namespace ftxui
// NOLINTEND

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
