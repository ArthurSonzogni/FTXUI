// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <string>  // for allocator

#include "ftxui/dom/elements.hpp"        // for text, flexbox
#include "ftxui/dom/flexbox_config.hpp"  // for FlexboxConfig, FlexboxConfig::Direction, FlexboxConfig::AlignContent, FlexboxConfig::JustifyContent, FlexboxConfig::Direction::Column, FlexboxConfig::AlignItems, FlexboxConfig::JustifyContent::SpaceAround, FlexboxConfig::AlignContent::Center, FlexboxConfig::AlignContent::FlexEnd, FlexboxConfig::AlignContent::SpaceAround, FlexboxConfig::AlignContent::SpaceBetween, FlexboxConfig::AlignContent::SpaceEvenly, FlexboxConfig::AlignItems::Center, FlexboxConfig::AlignItems::FlexEnd, FlexboxConfig::Direction::ColumnInversed, FlexboxConfig::Direction::Row, FlexboxConfig::Direction::RowInversed, FlexboxConfig::JustifyContent::Center, FlexboxConfig::JustifyContent::SpaceBetween
#include "ftxui/dom/node.hpp"            // for Render
#include "ftxui/screen/screen.hpp"       // for Screen

// NOLINTBEGIN
namespace ftxui {

TEST(FlexboxTest, BasicRow) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
      },
      FlexboxConfig().Set(FlexboxConfig::Direction::Row));
  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaabbb \r\n"
            "cccc   \r\n"
            "dddd   \r\n"
            "       ");
}

TEST(FlexboxTest, BasicRowInversed) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
      },
      FlexboxConfig().Set(FlexboxConfig::Direction::RowInversed));
  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " bbbaaa\r\n"
            "   cccc\r\n"
            "   dddd\r\n"
            "       ");
}

TEST(FlexboxTest, BasicColumn) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlexboxConfig().Set(FlexboxConfig::Direction::Column));

  Screen screen(8, 3);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaa dddd\r\n"
            "bbb e   \r\n"
            "cccc    ");
}

TEST(FlexboxTest, BasicColumnInversed) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlexboxConfig().Set(FlexboxConfig::Direction::ColumnInversed));

  Screen screen(8, 3);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "cccc    \r\n"
            "bbb e   \r\n"
            "aaa dddd");
}

TEST(FlexboxTest, JustifyContentCenter) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlexboxConfig().Set(FlexboxConfig::JustifyContent::Center));

  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaabbb \r\n"
            " cccc  \r\n"
            " dddde \r\n"
            "       ");
}

TEST(FlexboxTest, JustifyContentSpaceBetween) {
  auto root = flexbox(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlexboxConfig().Set(FlexboxConfig::JustifyContent::SpaceBetween));

  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaa bbb\r\n"
            "cccc   \r\n"
            "dddd  e\r\n"
            "       ");
}

TEST(FlexboxTest, JustifyContentSpaceAround) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("dddddddddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::JustifyContent::SpaceAround));

  Screen screen(15, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " aa  bb   ccc  \r\n"
            "ddddddddddddee \r\n"
            "  ff     ggg   \r\n"
            "               ");
}

TEST(FlexboxTest, JustifyContentSpaceEvenly) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("dddddddddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::JustifyContent::SpaceAround));

  Screen screen(15, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " aa  bb   ccc  \r\n"
            "ddddddddddddee \r\n"
            "  ff     ggg   \r\n"
            "               ");
}

TEST(FlexboxTest, AlignItemsFlexEnd) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig()
          .Set(FlexboxConfig::Direction::Column)
          .Set(FlexboxConfig::AlignItems::FlexEnd));

  Screen screen(15, 5);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "   aa ff       \r\n"
            "   bbggg       \r\n"
            "  ccc          \r\n"
            "ddddd          \r\n"
            "   ee          ");
}

TEST(FlexboxTest, AlignItemsCenter) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig()
          .Set(FlexboxConfig::Direction::Column)
          .Set(FlexboxConfig::AlignItems::Center));

  Screen screen(15, 5);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " aa  ff        \r\n"
            " bb  ggg       \r\n"
            " ccc           \r\n"
            "ddddd          \r\n"
            " ee            ");
}

TEST(FlexboxTest, AlignContentFlexEnd) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::AlignContent::FlexEnd));

  Screen screen(10, 5);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "          \r\n"
            "          \r\n"
            "aabbccc   \r\n"
            "dddddeeff \r\n"
            "ggg       ");
}

TEST(FlexboxTest, AlignContentCenter) {
  auto root = flexbox(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::AlignContent::Center));

  Screen screen(10, 5);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "          \r\n"
            "aabbccc   \r\n"
            "dddddeeff \r\n"
            "ggg       \r\n"
            "          ");
}

TEST(FlexboxTest, AlignContentSpaceBetween) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::AlignContent::SpaceBetween));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aabbb  \r\n"
            "       \r\n"
            "       \r\n"
            "ccc    \r\n"
            "       \r\n"
            "       \r\n"
            "dddddee\r\n"
            "       \r\n"
            "       \r\n"
            "ffggg  ");
}

TEST(FlexboxTest, AlignContentSpaceAround) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::AlignContent::SpaceAround));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aabbb  \r\n"
            "       \r\n"
            "ccc    \r\n"
            "       \r\n"
            "       \r\n"
            "dddddee\r\n"
            "       \r\n"
            "       \r\n"
            "ffggg  \r\n"
            "       ");
}

TEST(FlexboxTest, AlignContentSpaceEvenly) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().Set(FlexboxConfig::AlignContent::SpaceEvenly));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "       \r\n"
            "aabbb  \r\n"
            "       \r\n"
            "ccc    \r\n"
            "       \r\n"
            "dddddee\r\n"
            "       \r\n"
            "ffggg  \r\n"
            "       \r\n"
            "       ");
}

TEST(FlexboxTest, GapX) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().SetGap(1, 0));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aa bbb \r\n"
            "ccc    \r\n"
            "ddddd  \r\n"
            "ee ff  \r\n"
            "ggg    \r\n"
            "       \r\n"
            "       \r\n"
            "       \r\n"
            "       \r\n"
            "       ");
}

TEST(FlexboxTest, GapX2) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().SetGap(2, 0));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aa  bbb\r\n"
            "ccc    \r\n"
            "ddddd  \r\n"
            "ee  ff \r\n"
            "ggg    \r\n"
            "       \r\n"
            "       \r\n"
            "       \r\n"
            "       \r\n"
            "       ");
}

TEST(FlexboxTest, GapY) {
  auto root = flexbox(
      {
          text("aa"),
          text("bbb"),
          text("ccc"),
          text("ddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlexboxConfig().SetGap(0, 1));

  Screen screen(7, 10);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aabbb  \r\n"
            "       \r\n"
            "ccc    \r\n"
            "       \r\n"
            "dddddee\r\n"
            "       \r\n"
            "ffggg  \r\n"
            "       \r\n"
            "       \r\n"
            "       ");
}

TEST(FlexboxTest, Focus) {
  auto document = vbox({
                      paragraph("0 -"),
                      paragraph("1 -"),
                      paragraph("2 -"),
                      paragraph("3 -"),
                      paragraph("4 -"),
                      paragraph("5 -"),
                      paragraph("6 -"),
                      paragraph("7 -") | focus,
                      paragraph("8 -"),
                      paragraph("9 -"),
                  }) |
                  yframe | flex;

  Screen screen(1, 3);
  Render(screen, document);
  EXPECT_EQ(screen.ToString(),
            "-\r\n"
            "7\r\n"
            "-");
}

}  // namespace ftxui
// NOLINTEND
