#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>                   // for shared_ptr
#include <string>                   // for allocator, to_string, string
#include <utility>                  // for move

#include "ftxui/dom/elements.hpp"  // for operator|, Element, operator|=, text, vbox, Elements, border, focus, frame, vscroll_indicator
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for EXPECT_EQ, Test, TEST

namespace ftxui {

namespace {
Element MakeList(int focused_index, int n) {
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

std::string Print(int focused_index, int n) {
  auto element = MakeList(focused_index, n);
  Screen screen(6, 6);
  Render(screen, element);
  return screen.ToString();
}

}  // namespace

TEST(ScrollIndicator, Basic) {
  EXPECT_EQ(Print(0, 10),
            "╭────╮\r\n"
            "│0  ┃│\r\n"
            "│1  ┃│\r\n"
            "│2   │\r\n"
            "│3   │\r\n"
            "╰────╯");
  EXPECT_EQ(Print(1, 10),
            "╭────╮\r\n"
            "│0  ┃│\r\n"
            "│1  ┃│\r\n"
            "│2   │\r\n"
            "│3   │\r\n"
            "╰────╯");
  EXPECT_EQ(Print(2, 10),
            "╭────╮\r\n"
            "│1  ┃│\r\n"
            "│2  ┃│\r\n"
            "│3   │\r\n"
            "│4   │\r\n"
            "╰────╯");
  EXPECT_EQ(Print(3, 10),
            "╭────╮\r\n"
            "│2  ╻│\r\n"
            "│3  ┃│\r\n"
            "│4  ╹│\r\n"
            "│5   │\r\n"
            "╰────╯");
  EXPECT_EQ(Print(4, 10),
            "╭────╮\r\n"
            "│3   │\r\n"
            "│4  ┃│\r\n"
            "│5  ┃│\r\n"
            "│6   │\r\n"
            "╰────╯");
  EXPECT_EQ(Print(5, 10),
            "╭────╮\r\n"
            "│4   │\r\n"
            "│5  ╻│\r\n"
            "│6  ┃│\r\n"
            "│7  ╹│\r\n"
            "╰────╯");
  EXPECT_EQ(Print(6, 10),
            "╭────╮\r\n"
            "│5   │\r\n"
            "│6   │\r\n"
            "│7  ┃│\r\n"
            "│8  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(Print(7, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(Print(8, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
  EXPECT_EQ(Print(9, 10),
            "╭────╮\r\n"
            "│6   │\r\n"
            "│7   │\r\n"
            "│8  ┃│\r\n"
            "│9  ┃│\r\n"
            "╰────╯");
}

}  // namespace ftxui

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
