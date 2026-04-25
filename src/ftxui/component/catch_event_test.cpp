// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for CatchEvent, Renderer
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown
#include "ftxui/dom/elements.hpp"     // for text, Element
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, TEST

// NOLINTBEGIN
namespace ftxui {

TEST(CatchEventTest, BasicCatch) {
  int caught = 0;
  auto inner = Renderer([] { return text("hello"); });
  auto component = CatchEvent(inner, [&](Event event) {
    if (event == Event::Return) {
      caught++;
      return true;
    }
    return false;
  });

  EXPECT_EQ(caught, 0);
  EXPECT_TRUE(component->OnEvent(Event::Return));
  EXPECT_EQ(caught, 1);
}

TEST(CatchEventTest, UnhandledEventPassesThrough) {
  int caught = 0;
  int passed = 0;
  auto inner =
      CatchEvent(Renderer([] { return text("hello"); }), [&](Event /*event*/) {
        passed++;
        return false;
      });
  auto component = CatchEvent(inner, [&](Event event) {
    if (event == Event::Return) {
      caught++;
      return true;
    }
    return false;
  });

  // ArrowDown is not caught by the outer handler; check inner handler sees it.
  component->OnEvent(Event::ArrowDown);
  EXPECT_EQ(caught, 0);
  EXPECT_EQ(passed, 1);
}

TEST(CatchEventTest, OuterHandlerPreventsPropagation) {
  int inner_count = 0;
  int outer_count = 0;
  auto inner =
      CatchEvent(Renderer([] { return text("hello"); }), [&](Event /*event*/) {
        inner_count++;
        return false;
      });
  auto component = CatchEvent(inner, [&](Event event) {
    outer_count++;
    if (event == Event::Return) {
      return true;  // Consume the event.
    }
    return false;
  });

  // The outer handler catches Return and prevents inner from seeing it.
  component->OnEvent(Event::Return);
  EXPECT_EQ(outer_count, 1);
  EXPECT_EQ(inner_count, 0);
}

TEST(CatchEventTest, MultipleEvents) {
  int count = 0;
  auto inner = Renderer([] { return text("hello"); });
  auto component = CatchEvent(inner, [&](Event /*event*/) {
    count++;
    return true;
  });

  for (int i = 0; i < 5; i++) {
    component->OnEvent(Event::Return);
  }
  EXPECT_EQ(count, 5);
}

TEST(CatchEventTest, DecoratorSyntax) {
  int caught = 0;
  auto component = Renderer([] { return text("hello"); });
  component |= CatchEvent([&](Event event) {
    if (event == Event::Return) {
      caught++;
      return true;
    }
    return false;
  });

  component->OnEvent(Event::Return);
  EXPECT_EQ(caught, 1);
}

TEST(CatchEventTest, ReturnFalseAllowsPropagation) {
  int inner_count = 0;
  auto inner =
      CatchEvent(Renderer([] { return text("hello"); }), [&](Event /*event*/) {
        inner_count++;
        return false;
      });
  auto component = CatchEvent(inner, [](Event /*event*/) {
    return false;  // Don't consume.
  });

  component->OnEvent(Event::ArrowDown);
  EXPECT_EQ(inner_count, 1);
}

}  // namespace ftxui
// NOLINTEND
