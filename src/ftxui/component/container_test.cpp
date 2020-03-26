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
