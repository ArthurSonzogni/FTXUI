// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>   // for allocator, to_string, string
#include <utility>  // for move

#include "ftxui/dom/elements.hpp"  // for operator|, Element, operator|=, text, vbox, Elements, border, select, frame, vscroll_indicator
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::Red
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

namespace {
Element MakeVerticalList(int focused_index, int n) {
  Elements list;
  for (int i = 0; i < n; ++i) {
    auto element = text(std::to_string(i));
    if (i == focused_index) {
      element |= focus;
    }
    list.push_back(element);
  }
  return vbox(std::move(list)) | vscroll_indicator | frame | border;
}

Element MakeHorizontalList(int focused_index, int n) {
  Elements list;
  for (int i = 0; i < n; ++i) {
    auto element = text(std::to_string(i));
    if (i == focused_index) {
      element |= focus;
    }
    list.push_back(element);
  }
  return hbox(std::move(list)) | hscroll_indicator | frame | border;
}

std::string PrintVerticalList(int focused_index, int n) {
  auto element = MakeVerticalList(focused_index, n);
  Screen screen(6, 6);
  Render(screen, element);
  return screen.ToString();
}

std::string PrintHorizontalList(int focused_index, int n) {
  auto element = MakeHorizontalList(focused_index, n);
  Screen screen(6, 4);
  Render(screen, element);
  return screen.ToString();
}

}  // namespace

TEST(ScrollIndicator, BasicVertical) {
  EXPECT_EQ(PrintVerticalList(0, 10),
            "╭────╮\r\n"
            "│0  ┃│\r\n"
            "│1  ┃│\r\n"
            "│2   │\r\n"
            "│3   │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(1, 10),
            "╭────╮\r\n"
            "│0  ┃│\r\n"
            "│1  ┃│\r\n"
            "│2   │\r\n"
            "│3   │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(2, 10),
            "╭────╮\r\n"
            "│1  ┃│\r\n"
            "│2  ┃│\r\n"
            "│3   │\r\n"
            "│4   │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(3, 10),
            "╭────╮\r\n"
            "│2  ╻│\r\n"
            "│3  ┃│\r\n"
            "│4  ╹│\r\n"
            "│5   │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(4, 10),
            "╭────╮\r\n"
            "│3   │\r\n"
            "│4  ┃│\r\n"
            "│5  ┃│\r\n"
            "│6   │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(5, 10),
            "╭────╮\r\n"
            "│4   │\r\n"
            "│5  ╻│\r\n"
            "│6  ┃│\r\n"
            "│7  ╹│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(6, 10),
            "╭────╮\r\n"
            "│5   │\r\n"
            "│6   │\r\n"
            "│7  ┃│\r\n"
            "│8  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(7, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(8, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintVerticalList(9, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
}

TEST(ScrollIndicator, VerticalColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│0  ┃│\r\n"
  //           "│1  ┃│\r\n"
  //           "│2   │\r\n"
  //           "│3   │\r\n"
  //           "╰────╯"

  auto element = MakeVerticalList(0, 10) | color(Color::Red);
  Screen screen(6, 6);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 4).foreground_color, Color::Red);
  EXPECT_EQ(screen.PixelAt(4, 4).background_color, Color());
}

TEST(ScrollIndicator, VerticalBackgroundColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│0  ┃│\r\n"
  //           "│1  ┃│\r\n"
  //           "│2   │\r\n"
  //           "│3   │\r\n"
  //           "╰────╯"

  auto element = MakeVerticalList(0, 10) | bgcolor(Color::Red);
  Screen screen(6, 6);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 4).foreground_color, Color());
  EXPECT_EQ(screen.PixelAt(4, 4).background_color, Color::Red);
}

TEST(ScrollIndicator, VerticalFullColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│0  ┃│\r\n"
  //           "│1  ┃│\r\n"
  //           "│2   │\r\n"
  //           "│3   │\r\n"
  //           "╰────╯"

  auto element =
      MakeVerticalList(0, 10) | color(Color::Red) | bgcolor(Color::Red);
  Screen screen(6, 6);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 4).foreground_color, Color::Red);
  EXPECT_EQ(screen.PixelAt(4, 4).background_color, Color::Red);
}

TEST(ScrollIndicator, BasicHorizontal) {
  EXPECT_EQ(PrintHorizontalList(0, 10),
            "╭────╮\r\n"
            "│0123│\r\n"
            "│──  │\r\n"
            "╰────╯");

  EXPECT_EQ(PrintHorizontalList(1, 10),
            "╭────╮\r\n"
            "│0123│\r\n"
            "│──  │\r\n"
            "╰────╯");

  EXPECT_EQ(PrintHorizontalList(2, 10),
            "╭────╮\r\n"
            "│1234│\r\n"
            "│──  │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(3, 10),
            "╭────╮\r\n"
            "│2345│\r\n"
            "│╶─╴ │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(4, 10),
            "╭────╮\r\n"
            "│3456│\r\n"
            "│ ── │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(5, 10),
            "╭────╮\r\n"
            "│4567│\r\n"
            "│ ╶─╴│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(6, 10),
            "╭────╮\r\n"
            "│5678│\r\n"
            "│  ──│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(7, 10),
            "╭────╮\r\n"
            "│6789│\r\n"
            "│  ──│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalList(8, 10),
            "╭────╮\r\n"
            "│6789│\r\n"
            "│  ──│\r\n"
            "╰────╯");
}

TEST(ScrollIndicator, HorizontalColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│5678│\r\n"
  //           "│  ──│\r\n"
  //           "╰────╯"

  auto element = MakeHorizontalList(6, 10) | color(Color::Red);
  Screen screen(6, 4);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 2).foreground_color, Color::Red);
  EXPECT_EQ(screen.PixelAt(4, 2).background_color, Color());
}

TEST(ScrollIndicator, HorizontalBackgroundColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│5678│\r\n"
  //           "│  ──│\r\n"
  //           "╰────╯"

  auto element = MakeHorizontalList(6, 10) | bgcolor(Color::Red);
  Screen screen(6, 4);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 2).foreground_color, Color());
  EXPECT_EQ(screen.PixelAt(4, 2).background_color, Color::Red);
}

TEST(ScrollIndicator, HorizontalFullColorable) {
  // The list we generate looks like this
  //           "╭────╮\r\n"
  //           "│5678│\r\n"
  //           "│  ──│\r\n"
  //           "╰────╯"

  auto element =
      MakeHorizontalList(6, 10) | color(Color::Red) | bgcolor(Color::Red);
  Screen screen(6, 4);
  Render(screen, element);

  EXPECT_EQ(screen.PixelAt(4, 2).foreground_color, Color::Red);
  EXPECT_EQ(screen.PixelAt(4, 2).background_color, Color::Red);
}

namespace {

Element MakeHorizontalFlexboxList(int n) {
  Elements list;
  for (int i = 0; i < n; ++i) {
    list.push_back(text(std::to_string(i % 10)));
  }
  return flexbox(std::move(list)) | vscroll_indicator | yframe | border;
}

std::string PrintHorizontalFlexboxList(int n) {
  auto element = MakeHorizontalFlexboxList(n);
  Screen screen(6, 6);
  Render(screen, element);
  return screen.ToString();
}

TEST(ScrollIndicator, HorizontalFlexbox) {
  EXPECT_EQ(PrintHorizontalFlexboxList(1),
            "╭────╮\r\n"
            "│0   │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(2),
            "╭────╮\r\n"
            "│01  │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(3),
            "╭────╮\r\n"
            "│012 │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(4),
            "╭────╮\r\n"
            "│012 │\r\n"
            "│3   │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(5),
            "╭────╮\r\n"
            "│012 │\r\n"
            "│34  │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(6),
            "╭────╮\r\n"
            "│012 │\r\n"
            "│345 │\r\n"
            "│    │\r\n"
            "│    │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(11),
            "╭────╮\r\n"
            "│012 │\r\n"
            "│345 │\r\n"
            "│678 │\r\n"
            "│90  │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(15),
            "╭────╮\r\n"
            "│012┃│\r\n"
            "│345┃│\r\n"
            "│678┃│\r\n"
            "│901╹│\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(16),
            "╭────╮\r\n"
            "│012┃│\r\n"
            "│345┃│\r\n"
            "│678┃│\r\n"
            "│901 │\r\n"
            "╰────╯");
  EXPECT_EQ(PrintHorizontalFlexboxList(17),
            "╭────╮\r\n"
            "│012┃│\r\n"
            "│345┃│\r\n"
            "│678┃│\r\n"
            "│901 │\r\n"
            "╰────╯");
}

}  // namespace

}  // namespace ftxui
// NOLINTEND
