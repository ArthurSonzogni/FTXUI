// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Maybe, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"           // for Event, Event::Return
#include "ftxui/dom/elements.hpp"              // for text, Element
#include "ftxui/dom/node.hpp"                  // for Render
#include "ftxui/screen/screen.hpp"             // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

namespace {

Component MakeTestComponent() {
  return Renderer([] { return text("hello"); });
}

}  // namespace

TEST(MaybeTest, ShowWithBoolPointer) {
  bool show = true;
  auto inner = MakeTestComponent();
  auto component = Maybe(inner, &show);

  Screen screen(10, 1);
  Render(screen, component->Render());
  EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, HideWithBoolPointer) {
  bool show = false;
  auto inner = MakeTestComponent();
  auto component = Maybe(inner, &show);

  Screen screen(10, 1);
  Render(screen, component->Render());
  // When hidden, the rendered content should be empty.
  EXPECT_EQ(screen.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, DynamicToggle) {
  bool show = true;
  auto inner = MakeTestComponent();
  auto component = Maybe(inner, &show);

  {
    Screen screen(10, 1);
    Render(screen, component->Render());
    EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
  }

  show = false;
  {
    Screen screen(10, 1);
    Render(screen, component->Render());
    EXPECT_EQ(screen.ToString().find("hello"), std::string::npos);
  }

  show = true;
  {
    Screen screen(10, 1);
    Render(screen, component->Render());
    EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
  }
}

TEST(MaybeTest, ShowWithFunction) {
  bool show = true;
  auto inner = MakeTestComponent();
  auto component = Maybe(inner, [&] { return show; });

  Screen screen(10, 1);
  Render(screen, component->Render());
  EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, HideWithFunction) {
  bool show = false;
  auto inner = MakeTestComponent();
  auto component = Maybe(inner, [&] { return show; });

  Screen screen(10, 1);
  Render(screen, component->Render());
  EXPECT_EQ(screen.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, EventBlockedWhenHidden) {
  bool show = false;
  int event_count = 0;
  auto inner = Renderer([&] { return text("hello"); });
  inner = CatchEvent(inner, [&](Event /*event*/) {
    event_count++;
    return false;
  });
  auto component = Maybe(inner, &show);

  // Events should not propagate to the inner component when hidden.
  component->OnEvent(Event::Return);
  EXPECT_EQ(event_count, 0);
}

TEST(MaybeTest, EventPassedWhenVisible) {
  bool show = true;
  int event_count = 0;
  auto inner = Renderer([&] { return text("hello"); });
  inner = CatchEvent(inner, [&](Event /*event*/) {
    event_count++;
    return true;
  });
  auto component = Maybe(inner, &show);

  // Events should propagate to the inner component when visible.
  component->OnEvent(Event::Return);
  EXPECT_EQ(event_count, 1);
}

TEST(MaybeTest, DecoratorBoolPointer) {
  bool show = true;
  auto inner = MakeTestComponent();
  inner |= Maybe(&show);

  Screen screen(10, 1);
  Render(screen, inner->Render());
  EXPECT_NE(screen.ToString().find("hello"), std::string::npos);

  show = false;
  Screen screen2(10, 1);
  Render(screen2, inner->Render());
  EXPECT_EQ(screen2.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, DecoratorFunction) {
  bool show = true;
  auto inner = MakeTestComponent();
  inner |= Maybe([&] { return show; });

  Screen screen(10, 1);
  Render(screen, inner->Render());
  EXPECT_NE(screen.ToString().find("hello"), std::string::npos);
}

TEST(MaybeTest, FocusableWhenVisible) {
  bool show = true;
  int focused_count = 0;
  auto inner = Renderer([&](bool focused) {
    if (focused) {
      focused_count++;
    }
    return text("hello");
  });
  auto component = Maybe(inner, &show);

  // When visible, inner component should be focusable.
  EXPECT_TRUE(component->Focusable());
}

TEST(MaybeTest, NotFocusableWhenHidden) {
  bool show = false;
  auto inner = Renderer([](bool /*focused*/) { return text("hello"); });
  auto component = Maybe(inner, &show);

  // When hidden, component should not be focusable.
  EXPECT_FALSE(component->Focusable());
}

}  // namespace ftxui
// NOLINTEND
