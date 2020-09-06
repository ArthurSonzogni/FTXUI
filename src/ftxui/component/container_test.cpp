#include "ftxui/component/container.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

TEST(ContainerTest, HorizontalEvent) {
  auto container = Container::Horizontal();
  Component c0, c1, c2;
  container.Add(&c0);
  container.Add(&c1);
  container.Add(&c2);

  // With arrow key.
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowRight);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::ArrowRight);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::ArrowRight);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With arrow key in the wrong dimension.
  container.OnEvent(Event::ArrowUp);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowDown);
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With vim like characters.
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('l'));
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Character('l'));
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Character('l'));
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Character('h'));
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Character('h'));
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('h'));
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With vim like characters in the wrong direction.
  container.OnEvent(Event::Character('j'));
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('k'));
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With tab characters.
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::TabReverse);
}

TEST(ContainerTest, VerticalEvent) {
  auto container = Container::Vertical();
  Component c0, c1, c2;
  container.Add(&c0);
  container.Add(&c1);
  container.Add(&c2);

  // With arrow key.
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowDown);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::ArrowDown);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::ArrowDown);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::ArrowUp);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::ArrowUp);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowUp);
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With arrow key in the wrong dimension.
  container.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::ArrowRight);
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With vim like characters.
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('j'));
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Character('j'));
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Character('j'));
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Character('k'));
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Character('k'));
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('k'));
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With vim like characters in the wrong direction.
  container.OnEvent(Event::Character('h'));
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Character('l'));
  EXPECT_EQ(container.ActiveChild(), &c0);

  // With tab characters.
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::Tab);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c0);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c2);
  container.OnEvent(Event::TabReverse);
  EXPECT_EQ(container.ActiveChild(), &c1);
  container.OnEvent(Event::TabReverse);
}

TEST(ContainerTest, SetActiveChild) {
  auto container = Container::Horizontal();
  Component c0, c1, c2;
  container.Add(&c0);
  container.Add(&c1);
  container.Add(&c2);

  EXPECT_EQ(container.ActiveChild(), &c0);
  EXPECT_TRUE(c0.Focused());
  EXPECT_TRUE(c0.Active());
  EXPECT_FALSE(c1.Focused());
  EXPECT_FALSE(c1.Active());
  EXPECT_FALSE(c2.Focused());
  EXPECT_FALSE(c2.Active());

  container.SetActiveChild(&c0);
  EXPECT_EQ(container.ActiveChild(), &c0);
  EXPECT_TRUE(c0.Focused());
  EXPECT_TRUE(c0.Active());
  EXPECT_FALSE(c1.Focused());
  EXPECT_FALSE(c1.Active());
  EXPECT_FALSE(c2.Focused());
  EXPECT_FALSE(c2.Active());

  container.SetActiveChild(&c1);
  EXPECT_EQ(container.ActiveChild(), &c1);
  EXPECT_FALSE(c0.Focused());
  EXPECT_FALSE(c0.Active());
  EXPECT_TRUE(c1.Focused());
  EXPECT_TRUE(c1.Active());
  EXPECT_FALSE(c2.Focused());
  EXPECT_FALSE(c2.Active());

  container.SetActiveChild(&c2);
  EXPECT_EQ(container.ActiveChild(), &c2);
  EXPECT_FALSE(c0.Focused());
  EXPECT_FALSE(c0.Active());
  EXPECT_FALSE(c1.Focused());
  EXPECT_FALSE(c1.Active());
  EXPECT_TRUE(c2.Focused());
  EXPECT_TRUE(c2.Active());

  container.SetActiveChild(&c0);
  EXPECT_EQ(container.ActiveChild(), &c0);
  EXPECT_TRUE(c0.Focused());
  EXPECT_TRUE(c0.Active());
  EXPECT_FALSE(c1.Focused());
  EXPECT_FALSE(c1.Active());
  EXPECT_FALSE(c2.Focused());
  EXPECT_FALSE(c2.Active());
}

TEST(ContainerTest, TakeFocus) {
  auto c = Container::Horizontal();
  auto c1 = Container::Vertical();
  auto c2 = Container::Vertical();
  auto c3 = Container::Vertical();
  auto c11 = Container::Horizontal();
  auto c12 = Container::Horizontal();
  auto c13 = Container::Horizontal();
  auto c21 = Container::Horizontal();
  auto c22 = Container::Horizontal();
  auto c23 = Container::Horizontal();

  c.Add(&c1);
  c.Add(&c2);
  c.Add(&c3);
  c1.Add(&c11);
  c1.Add(&c12);
  c1.Add(&c13);
  c2.Add(&c21);
  c2.Add(&c22);
  c2.Add(&c23);

  EXPECT_TRUE(c.Focused());
  EXPECT_TRUE(c1.Focused());
  EXPECT_FALSE(c2.Focused());
  EXPECT_TRUE(c11.Focused());
  EXPECT_FALSE(c12.Focused());
  EXPECT_FALSE(c13.Focused());
  EXPECT_FALSE(c21.Focused());
  EXPECT_FALSE(c22.Focused());
  EXPECT_FALSE(c23.Focused());
  EXPECT_TRUE(c.Active());
  EXPECT_TRUE(c1.Active());
  EXPECT_FALSE(c2.Active());
  EXPECT_TRUE(c11.Active());
  EXPECT_FALSE(c12.Active());
  EXPECT_FALSE(c13.Active());
  EXPECT_TRUE(c21.Active());
  EXPECT_FALSE(c22.Active());
  EXPECT_FALSE(c23.Active());

  c22.TakeFocus();
  EXPECT_TRUE(c.Focused());
  EXPECT_FALSE(c1.Focused());
  EXPECT_TRUE(c2.Focused());
  EXPECT_FALSE(c11.Focused());
  EXPECT_FALSE(c12.Focused());
  EXPECT_FALSE(c13.Focused());
  EXPECT_FALSE(c21.Focused());
  EXPECT_TRUE(c22.Focused());
  EXPECT_FALSE(c23.Focused());
  EXPECT_TRUE(c.Active());
  EXPECT_FALSE(c1.Active());
  EXPECT_TRUE(c2.Active());
  EXPECT_TRUE(c11.Active());
  EXPECT_FALSE(c12.Active());
  EXPECT_FALSE(c13.Active());
  EXPECT_FALSE(c21.Active());
  EXPECT_TRUE(c22.Active());
  EXPECT_FALSE(c23.Active());

  c1.TakeFocus();
  EXPECT_TRUE(c.Focused());
  EXPECT_TRUE(c1.Focused());
  EXPECT_FALSE(c2.Focused());
  EXPECT_TRUE(c11.Focused());
  EXPECT_FALSE(c12.Focused());
  EXPECT_FALSE(c13.Focused());
  EXPECT_FALSE(c21.Focused());
  EXPECT_FALSE(c22.Focused());
  EXPECT_FALSE(c23.Focused());
  EXPECT_TRUE(c.Active());
  EXPECT_TRUE(c1.Active());
  EXPECT_FALSE(c2.Active());
  EXPECT_TRUE(c11.Active());
  EXPECT_FALSE(c12.Active());
  EXPECT_FALSE(c13.Active());
  EXPECT_FALSE(c21.Active());
  EXPECT_TRUE(c22.Active());
  EXPECT_FALSE(c23.Active());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
