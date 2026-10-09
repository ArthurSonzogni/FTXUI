// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/component.hpp"  // for Horizontal, Vertical, Button, Tab
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/event.hpp"  // for Event, Event::Tab, Event::TabReverse, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::WheelDown
#include "ftxui/dom/elements.hpp"     // for vbox, dbox, yframe, size, separator
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_FALSE, Test, EXPECT_TRUE, TEST

namespace ftxui {

namespace {
Component Focusable() {
  return Button("", [] {});
}
Component NonFocusable() {
  return Container::Horizontal({});
}
}  // namespace

TEST(ContainerTest, HorizontalEvent) {
  auto container = Container::Horizontal({});
  auto c0 = Focusable();
  auto c1 = Focusable();
  auto c2 = Focusable();
  container->Add(c0);
  container->Add(c1);
  container->Add(NonFocusable());
  container->Add(NonFocusable());
  container->Add(c2);
  container->Add(NonFocusable());

  // With arrow key.
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container->ActiveChild(), c0);

  // With arrow key in the wrong dimension.
  container->OnEvent(Event::ArrowUp);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowDown);
  EXPECT_EQ(container->ActiveChild(), c0);

  // With vim like characters.
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('l'));
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Character('l'));
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Character('l'));
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Character('h'));
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Character('h'));
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('h'));
  EXPECT_EQ(container->ActiveChild(), c0);

  // With vim like characters in the wrong direction.
  container->OnEvent(Event::Character('j'));
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('k'));
  EXPECT_EQ(container->ActiveChild(), c0);

  // With tab characters.
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::TabReverse);
}

TEST(ContainerTest, VerticalEvent) {
  auto container = Container::Vertical({});
  auto c0 = Focusable();
  auto c1 = Focusable();
  auto c2 = Focusable();
  container->Add(c0);
  container->Add(c1);
  container->Add(NonFocusable());
  container->Add(NonFocusable());
  container->Add(c2);
  container->Add(NonFocusable());

  // With arrow key.
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowDown);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::ArrowDown);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::ArrowDown);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::ArrowUp);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::ArrowUp);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowUp);
  EXPECT_EQ(container->ActiveChild(), c0);

  // With arrow key in the wrong dimension.
  container->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::ArrowRight);
  EXPECT_EQ(container->ActiveChild(), c0);

  // With vim like characters.
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('j'));
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Character('j'));
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Character('j'));
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Character('k'));
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Character('k'));
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('k'));
  EXPECT_EQ(container->ActiveChild(), c0);

  // With vim like characters in the wrong direction.
  container->OnEvent(Event::Character('h'));
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Character('l'));
  EXPECT_EQ(container->ActiveChild(), c0);

  // With tab characters.
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::Tab);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c0);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c2);
  container->OnEvent(Event::TabReverse);
  EXPECT_EQ(container->ActiveChild(), c1);
  container->OnEvent(Event::TabReverse);
}

TEST(ContainerTest, InitializeWithFocusableChild) {
  auto button = Focusable();
  auto inner = Container::Vertical({NonFocusable(), button});
  auto outer = Container::Vertical({Focusable(), inner});

  outer->OnEvent(Event::ArrowDown);

  EXPECT_EQ(inner->ActiveChild(), button);
  EXPECT_TRUE(button->Focused());
}

TEST(ContainerTest, HorizontalUpdatesDynamicallyFocusableSelection) {
  bool show_first = true;
  auto first = Focusable();
  auto maybe_first = Maybe(first, &show_first);
  auto second = Focusable();
  auto container = Container::Horizontal({maybe_first, second});

  EXPECT_EQ(container->ActiveChild(), maybe_first);
  EXPECT_TRUE(first->Focused());

  show_first = false;
  container->Render();

  EXPECT_EQ(container->ActiveChild(), second);
  EXPECT_FALSE(first->Focused());
  EXPECT_TRUE(second->Focused());
}

TEST(ContainerTest, VerticalUpdatesDynamicallyFocusableSelection) {
  bool show_first = true;
  auto first = Focusable();
  auto maybe_first = Maybe(first, &show_first);
  auto second = Focusable();
  auto container = Container::Vertical({maybe_first, second});

  EXPECT_EQ(container->ActiveChild(), maybe_first);
  EXPECT_TRUE(first->Focused());

  show_first = false;
  container->Render();

  EXPECT_EQ(container->ActiveChild(), second);
  EXPECT_FALSE(first->Focused());
  EXPECT_TRUE(second->Focused());
}

TEST(ContainerTest, SetActiveChild) {
  auto container = Container::Horizontal({});
  auto c0 = Focusable();
  auto c1 = Focusable();
  auto c2 = Focusable();
  container->Add(c0);
  container->Add(c1);
  container->Add(c2);

  EXPECT_EQ(container->ActiveChild(), c0);
  EXPECT_TRUE(c0->Focused());
  EXPECT_TRUE(c0->Active());
  EXPECT_FALSE(c1->Focused());
  EXPECT_FALSE(c1->Active());
  EXPECT_FALSE(c2->Focused());
  EXPECT_FALSE(c2->Active());

  container->SetActiveChild(c0);
  EXPECT_EQ(container->ActiveChild(), c0);
  EXPECT_TRUE(c0->Focused());
  EXPECT_TRUE(c0->Active());
  EXPECT_FALSE(c1->Focused());
  EXPECT_FALSE(c1->Active());
  EXPECT_FALSE(c2->Focused());
  EXPECT_FALSE(c2->Active());

  container->SetActiveChild(c1);
  EXPECT_EQ(container->ActiveChild(), c1);
  EXPECT_FALSE(c0->Focused());
  EXPECT_FALSE(c0->Active());
  EXPECT_TRUE(c1->Focused());
  EXPECT_TRUE(c1->Active());
  EXPECT_FALSE(c2->Focused());
  EXPECT_FALSE(c2->Active());

  container->SetActiveChild(c2);
  EXPECT_EQ(container->ActiveChild(), c2);
  EXPECT_FALSE(c0->Focused());
  EXPECT_FALSE(c0->Active());
  EXPECT_FALSE(c1->Focused());
  EXPECT_FALSE(c1->Active());
  EXPECT_TRUE(c2->Focused());
  EXPECT_TRUE(c2->Active());

  container->SetActiveChild(c0);
  EXPECT_EQ(container->ActiveChild(), c0);
  EXPECT_TRUE(c0->Focused());
  EXPECT_TRUE(c0->Active());
  EXPECT_FALSE(c1->Focused());
  EXPECT_FALSE(c1->Active());
  EXPECT_FALSE(c2->Focused());
  EXPECT_FALSE(c2->Active());
}

TEST(ContainerTest, TakeFocus) {
  auto c = Container::Horizontal({});
  auto c1 = Container::Vertical({});
  auto c2 = Container::Vertical({});
  auto c3 = Container::Vertical({});
  auto c11 = Focusable();
  auto c12 = Focusable();
  auto c13 = Focusable();
  auto c21 = Focusable();
  auto c22 = Focusable();
  auto c23 = Focusable();

  c->Add(c1);
  c->Add(c2);
  c->Add(c3);
  c1->Add(c11);
  c1->Add(c12);
  c1->Add(c13);
  c2->Add(c21);
  c2->Add(c22);
  c2->Add(c23);

  EXPECT_TRUE(c->Focused());
  EXPECT_TRUE(c1->Focused());
  EXPECT_FALSE(c2->Focused());
  EXPECT_TRUE(c11->Focused());
  EXPECT_FALSE(c12->Focused());
  EXPECT_FALSE(c13->Focused());
  EXPECT_FALSE(c21->Focused());
  EXPECT_FALSE(c22->Focused());
  EXPECT_FALSE(c23->Focused());
  EXPECT_TRUE(c->Active());
  EXPECT_TRUE(c1->Active());
  EXPECT_FALSE(c2->Active());
  EXPECT_TRUE(c11->Active());
  EXPECT_FALSE(c12->Active());
  EXPECT_FALSE(c13->Active());
  EXPECT_TRUE(c21->Active());
  EXPECT_FALSE(c22->Active());
  EXPECT_FALSE(c23->Active());

  c22->TakeFocus();
  EXPECT_TRUE(c->Focused());
  EXPECT_FALSE(c1->Focused());
  EXPECT_TRUE(c2->Focused());
  EXPECT_FALSE(c11->Focused());
  EXPECT_FALSE(c12->Focused());
  EXPECT_FALSE(c13->Focused());
  EXPECT_FALSE(c21->Focused());
  EXPECT_TRUE(c22->Focused());
  EXPECT_FALSE(c23->Focused());
  EXPECT_TRUE(c->Active());
  EXPECT_FALSE(c1->Active());
  EXPECT_TRUE(c2->Active());
  EXPECT_TRUE(c11->Active());
  EXPECT_FALSE(c12->Active());
  EXPECT_FALSE(c13->Active());
  EXPECT_FALSE(c21->Active());
  EXPECT_TRUE(c22->Active());
  EXPECT_FALSE(c23->Active());

  c1->TakeFocus();
  EXPECT_TRUE(c->Focused());
  EXPECT_TRUE(c1->Focused());
  EXPECT_FALSE(c2->Focused());
  EXPECT_TRUE(c11->Focused());
  EXPECT_FALSE(c12->Focused());
  EXPECT_FALSE(c13->Focused());
  EXPECT_FALSE(c21->Focused());
  EXPECT_FALSE(c22->Focused());
  EXPECT_FALSE(c23->Focused());
  EXPECT_TRUE(c->Active());
  EXPECT_TRUE(c1->Active());
  EXPECT_FALSE(c2->Active());
  EXPECT_TRUE(c11->Active());
  EXPECT_FALSE(c12->Active());
  EXPECT_FALSE(c13->Active());
  EXPECT_FALSE(c21->Active());
  EXPECT_TRUE(c22->Active());
  EXPECT_FALSE(c23->Active());
}

TEST(ContainerTest, TabFocusable) {
  int selected = 0;
  auto c = Container::Tab(
      {
          Focusable(),
          NonFocusable(),
          Focusable(),
          NonFocusable(),
      },
      &selected);

  selected = 0;
  EXPECT_TRUE(c->Focusable());
  EXPECT_TRUE(c->Focused());

  selected = 1;
  EXPECT_FALSE(c->Focusable());
  EXPECT_FALSE(c->Focused());

  selected = 2;
  EXPECT_TRUE(c->Focusable());
  EXPECT_TRUE(c->Focused());

  selected = 3;
  EXPECT_FALSE(c->Focusable());
  EXPECT_FALSE(c->Focused());
}

// Bug #1377: A Renderer rendering descendants directly, bypassing the
// intermediate components.
namespace {
struct BypassedScroll {
  Component button_1 = Button("Button 1", [] {});
  Component button_2 = Button("Button 2", [] {});
  Component button_3 = Button("Button 3", [] {});
  Component inner = Container::Vertical({
      Container::Stacked({button_1}),
      Container::Stacked({button_2}),
      Container::Stacked({button_3}),
  });
  Component inner_renderer = Renderer(inner, [this] {
    return vbox({
               dbox({button_1->Render() | size(HEIGHT, EQUAL, 5)}),
               dbox({button_2->Render() | size(HEIGHT, EQUAL, 5)}),
               dbox({button_3->Render() | size(HEIGHT, EQUAL, 5)}),
           }) |
           yframe | size(HEIGHT, EQUAL, 8);
  });
  Component button = Button("Click Me", [] {});
  Component root = Container::Vertical({inner_renderer, button});
  Component root_renderer = Renderer(root, [this] {
    return vbox({inner_renderer->Render(), separator(), button->Render()});
  });

  std::string Draw() {
    auto screen = Screen::Create(Dimension::Fixed(20), Dimension::Fixed(12));
    Render(screen, root_renderer->Render());
    return screen.ToString();
  }
};
}  // namespace

TEST(ContainerTest, BypassedScrollKeepsActiveChildVisible) {
  BypassedScroll ui;
  ui.button_3->TakeFocus();
  EXPECT_NE(ui.Draw().find("Button 3"), std::string::npos);

  // Moving the focus out of the frame must keep the active child visible.
  ui.button->TakeFocus();
  EXPECT_NE(ui.Draw().find("Button 3"), std::string::npos);
}

TEST(ContainerTest, BypassedScrollMouseWheel) {
  BypassedScroll ui;
  ui.Draw();

  Mouse mouse;
  mouse.button = Mouse::WheelDown;
  mouse.motion = Mouse::Pressed;
  mouse.x = 1;
  mouse.y = 1;
  EXPECT_TRUE(ui.root_renderer->OnEvent(Event::Mouse("", mouse)));
  EXPECT_EQ(ui.inner->ActiveChild()->ActiveChild(), ui.button_2);
}

// The common pattern: a Renderer rendering the children of its container.
TEST(ContainerTest, BypassedContainerMouseWheel) {
  auto button_1 = Button("Button 1", [] {});
  auto button_2 = Button("Button 2", [] {});
  auto container = Container::Vertical({button_1, button_2});
  auto renderer = Renderer(container, [&] {
    return vbox({button_1->Render(), button_2->Render()});
  });

  auto screen = Screen::Create(Dimension::Fixed(20), Dimension::Fixed(6));
  Render(screen, renderer->Render());

  Mouse mouse;
  mouse.button = Mouse::WheelDown;
  mouse.motion = Mouse::Pressed;
  mouse.x = 1;
  mouse.y = 1;
  EXPECT_TRUE(renderer->OnEvent(Event::Mouse("", mouse)));
  EXPECT_EQ(container->ActiveChild(), button_2);

  // Outside of the container.
  mouse.button = Mouse::WheelUp;
  mouse.y = 6;
  EXPECT_FALSE(renderer->OnEvent(Event::Mouse("", mouse)));
  EXPECT_EQ(container->ActiveChild(), button_2);
}

// The area of a container scrolled in a frame is clipped to the frame.
TEST(ContainerTest, MouseWheelClippedByFrame) {
  auto container = Container::Vertical({
      Button(
          "1", [] {}, ButtonOption::Ascii()),
      Button(
          "2", [] {}, ButtonOption::Ascii()),
      Button(
          "3", [] {}, ButtonOption::Ascii()),
      Button(
          "4", [] {}, ButtonOption::Ascii()),
      Button(
          "5", [] {}, ButtonOption::Ascii()),
  });
  auto renderer = Renderer(container, [&] {
    return vbox({container->Render() | yframe | size(HEIGHT, EQUAL, 3)});
  });

  auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(6));
  Render(screen, renderer->Render());

  Mouse mouse;
  mouse.button = Mouse::WheelDown;
  mouse.motion = Mouse::Pressed;
  mouse.x = 1;

  // Below the frame, where the hidden children would be.
  mouse.y = 4;
  EXPECT_FALSE(renderer->OnEvent(Event::Mouse("", mouse)));
  EXPECT_EQ(container->ActiveChild(), container->ChildAt(0));

  // Inside the frame.
  mouse.y = 1;
  EXPECT_TRUE(renderer->OnEvent(Event::Mouse("", mouse)));
  EXPECT_EQ(container->ActiveChild(), container->ChildAt(1));
}

}  // namespace ftxui
