#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for collapsible
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for MenuOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowDown, Event::Return
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

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

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
