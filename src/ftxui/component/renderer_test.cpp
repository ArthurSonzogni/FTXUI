// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event
#include "ftxui/component/mouse.hpp"           // for Mouse
#include "ftxui/dom/elements.hpp"   // for text, bold, inverted, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

namespace {

Event MousePressed(int x, int y) {
  Mouse mouse;
  mouse.button = Mouse::Left;
  mouse.motion = Mouse::Pressed;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;
  mouse.x = x;
  mouse.y = y;
  return Event::Mouse("jjj", mouse);
}

}  // namespace

TEST(RendererTest, BasicRender) {
  auto component = Renderer([] { return text("hello"); });
  Screen screen(10, 1);
  Render(screen, component->Render());
  EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
}

TEST(RendererTest, RenderWithChild) {
  // Renderer with a child component forwards events but overrides rendering.
  int event_count = 0;
  auto child =
      CatchEvent(Renderer([] { return text("child"); }), [&](Event /*event*/) {
        event_count++;
        return true;
      });
  auto component = Renderer(child, [] { return text("override"); });

  Screen screen(10, 1);
  Render(screen, component->Render());
  // Should show "override", not "child".
  EXPECT_NE(screen.ToString().find("override"), std::string::npos);
  EXPECT_EQ(screen.ToString().find("child"), std::string::npos);

  // Events should still propagate to the child.
  component->OnEvent(Event::Return);
  EXPECT_EQ(event_count, 1);
}

TEST(RendererTest, FocusableRenderer) {
  bool focused = false;
  auto component = Renderer([&](bool f) {
    focused = f;
    return text("hello");
  });

  EXPECT_TRUE(component->Focusable());
  Screen screen(10, 1);
  Render(screen, component->Render());
  // After rendering with focus, the focused flag should be set.
  EXPECT_TRUE(focused);
}

TEST(RendererTest, ElementDecoratorSyntax) {
  auto component = Renderer([] { return text("hello"); });
  component |= Renderer(bold);

  Screen screen(10, 1);
  Render(screen, component->Render());
  EXPECT_TRUE(screen.CellAt(0, 0).bold);
}

TEST(RendererTest, FocusableMouseInteraction) {
  auto component = Renderer([](bool /*focused*/) { return text("hello"); });

  Screen screen(10, 1);
  Render(screen, component->Render());

  // Mouse press inside the component should take focus.
  component->OnEvent(MousePressed(0, 0));
  EXPECT_TRUE(component->Focused());
}

TEST(RendererTest, NonFocusableSimpleRenderer) {
  // Basic Renderer (without bool parameter) is not focusable.
  auto component = Renderer([] { return text("hello"); });
  EXPECT_FALSE(component->Focusable());
}

TEST(RendererTest, RendersCorrectContent) {
  std::string content = "dynamic content";
  auto component = Renderer([&] { return text(content); });

  {
    Screen screen(20, 1);
    Render(screen, component->Render());
    EXPECT_NE(screen.ToString().find("dynamic content"), std::string::npos);
  }

  content = "changed";
  {
    Screen screen(20, 1);
    Render(screen, component->Render());
    EXPECT_NE(screen.ToString().find("changed"), std::string::npos);
    EXPECT_EQ(screen.ToString().find("dynamic content"), std::string::npos);
  }
}

}  // namespace ftxui
// NOLINTEND
