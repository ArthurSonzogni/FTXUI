// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string

#include "ftxui/dom/elements.hpp"  // for paragraph, paragraphAlignLeft, paragraphAlignRight, paragraphAlignCenter, paragraphAlignJustify, Element
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for Test, EXPECT_EQ, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(ParagraphTest, Empty) {
  auto element = paragraph("");
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "          ");
}

TEST(ParagraphTest, SingleWord) {
  auto element = paragraph("hello");
  Screen screen(10, 1);
  Render(screen, element);
  EXPECT_EQ(screen.ToString(), "hello     ");
}

TEST(ParagraphTest, MultipleWords) {
  auto element = paragraph("hello world");
  Screen screen(10, 2);
  Render(screen, element);
  // "hello" and "world" should appear in the rendered output.
  std::string output = screen.ToString();
  EXPECT_NE(output.find("hello"), std::string::npos);
  EXPECT_NE(output.find("world"), std::string::npos);
}

TEST(ParagraphTest, AlignLeft) {
  auto element = paragraphAlignLeft("hello world");
  Screen screen(10, 2);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("hello"), std::string::npos);
  EXPECT_NE(output.find("world"), std::string::npos);
}

TEST(ParagraphTest, AlignRight) {
  auto element = paragraphAlignRight("hello world");
  Screen screen(10, 2);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("hello"), std::string::npos);
  EXPECT_NE(output.find("world"), std::string::npos);
}

TEST(ParagraphTest, AlignCenter) {
  auto element = paragraphAlignCenter("hello world");
  Screen screen(10, 2);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("hello"), std::string::npos);
  EXPECT_NE(output.find("world"), std::string::npos);
}

TEST(ParagraphTest, AlignJustify) {
  auto element = paragraphAlignJustify("hello world");
  Screen screen(10, 2);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("hello"), std::string::npos);
  EXPECT_NE(output.find("world"), std::string::npos);
}

TEST(ParagraphTest, MultiLineParagraph) {
  // Newlines in the paragraph text should create separate lines.
  auto element = paragraph("line1\nline2");
  Screen screen(10, 2);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("line1"), std::string::npos);
  EXPECT_NE(output.find("line2"), std::string::npos);
}

TEST(ParagraphTest, WordWrap) {
  // Long text should wrap to fit within the given width.
  auto element = paragraph("one two three");
  Screen screen(5, 3);
  Render(screen, element);
  std::string output = screen.ToString();
  EXPECT_NE(output.find("one"), std::string::npos);
  EXPECT_NE(output.find("two"), std::string::npos);
  EXPECT_NE(output.find("three"), std::string::npos);
}

}  // namespace ftxui
// NOLINTEND
