// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <ftxui/dom/flexbox_config.hpp>  // for FlexboxConfig, FlexboxConfig::Direction, FlexboxConfig::Direction::Column, FlexboxConfig::Direction::ColumnInversed, FlexboxConfig::Direction::Row, FlexboxConfig::Direction::RowInversed

#include "ftxui/dom/flexbox_helper.hpp"

// NOLINTBEGIN
namespace ftxui {

TEST(FlexboxHelperTest, BasicRow) {
  flexbox_helper::Block block_10_5;
  block_10_5.min_size_x = 10;
  block_10_5.min_size_y = 5;

  flexbox_helper::Global g;
  g.blocks = {
      block_10_5, block_10_5, block_10_5, block_10_5, block_10_5,
  };

  g.size_x = 32;
  g.size_y = 16;
  g.config = FlexboxConfig().Set(FlexboxConfig::Direction::Row);
  flexbox_helper::Compute(g);

  EXPECT_EQ(g.blocks.size(), 5u);

  EXPECT_EQ(g.blocks[0].line, 0);
  EXPECT_EQ(g.blocks[1].line, 0);
  EXPECT_EQ(g.blocks[2].line, 0);
  EXPECT_EQ(g.blocks[3].line, 1);
  EXPECT_EQ(g.blocks[4].line, 1);

  EXPECT_EQ(g.blocks[0].line_position, 0);
  EXPECT_EQ(g.blocks[1].line_position, 1);
  EXPECT_EQ(g.blocks[2].line_position, 2);
  EXPECT_EQ(g.blocks[3].line_position, 0);
  EXPECT_EQ(g.blocks[4].line_position, 1);

  EXPECT_EQ(g.blocks[0].x, 0);
  EXPECT_EQ(g.blocks[0].y, 0);
  EXPECT_EQ(g.blocks[0].dim_x, 10);
  EXPECT_EQ(g.blocks[0].dim_y, 5);

  EXPECT_EQ(g.blocks[1].x, 10);
  EXPECT_EQ(g.blocks[1].y, 0);
  EXPECT_EQ(g.blocks[1].dim_x, 10);
  EXPECT_EQ(g.blocks[1].dim_y, 5);

  EXPECT_EQ(g.blocks[2].x, 20);
  EXPECT_EQ(g.blocks[2].y, 0);
  EXPECT_EQ(g.blocks[2].dim_x, 10);
  EXPECT_EQ(g.blocks[2].dim_y, 5);

  EXPECT_EQ(g.blocks[3].x, 0);
  EXPECT_EQ(g.blocks[3].y, 5);
  EXPECT_EQ(g.blocks[3].dim_x, 10);
  EXPECT_EQ(g.blocks[3].dim_y, 5);

  EXPECT_EQ(g.blocks[4].x, 10);
  EXPECT_EQ(g.blocks[4].y, 5);
  EXPECT_EQ(g.blocks[4].dim_x, 10);
  EXPECT_EQ(g.blocks[4].dim_y, 5);
}

TEST(FlexboxHelperTest, BasicColumn) {
  flexbox_helper::Block block_10_5;
  block_10_5.min_size_x = 10;
  block_10_5.min_size_y = 5;

  flexbox_helper::Global g;
  g.blocks = {
      block_10_5, block_10_5, block_10_5, block_10_5, block_10_5,
  };

  g.size_x = 32;
  g.size_y = 16;
  g.config = FlexboxConfig().Set(FlexboxConfig::Direction::Column);
  flexbox_helper::Compute(g);

  EXPECT_EQ(g.blocks.size(), 5u);

  EXPECT_EQ(g.blocks[0].line, 0);
  EXPECT_EQ(g.blocks[1].line, 0);
  EXPECT_EQ(g.blocks[2].line, 0);
  EXPECT_EQ(g.blocks[3].line, 1);
  EXPECT_EQ(g.blocks[4].line, 1);

  EXPECT_EQ(g.blocks[0].line_position, 0);
  EXPECT_EQ(g.blocks[1].line_position, 1);
  EXPECT_EQ(g.blocks[2].line_position, 2);
  EXPECT_EQ(g.blocks[3].line_position, 0);
  EXPECT_EQ(g.blocks[4].line_position, 1);

  EXPECT_EQ(g.blocks[0].x, 0);
  EXPECT_EQ(g.blocks[0].y, 0);
  EXPECT_EQ(g.blocks[0].dim_x, 10);
  EXPECT_EQ(g.blocks[0].dim_y, 5);

  EXPECT_EQ(g.blocks[1].x, 0);
  EXPECT_EQ(g.blocks[1].y, 5);
  EXPECT_EQ(g.blocks[1].dim_x, 10);
  EXPECT_EQ(g.blocks[1].dim_y, 5);

  EXPECT_EQ(g.blocks[2].x, 0);
  EXPECT_EQ(g.blocks[2].y, 10);
  EXPECT_EQ(g.blocks[2].dim_x, 10);
  EXPECT_EQ(g.blocks[2].dim_y, 5);

  EXPECT_EQ(g.blocks[3].x, 10);
  EXPECT_EQ(g.blocks[3].y, 0);
  EXPECT_EQ(g.blocks[3].dim_x, 10);
  EXPECT_EQ(g.blocks[3].dim_y, 5);

  EXPECT_EQ(g.blocks[4].x, 10);
  EXPECT_EQ(g.blocks[4].y, 5);
  EXPECT_EQ(g.blocks[4].dim_x, 10);
  EXPECT_EQ(g.blocks[4].dim_y, 5);
}

TEST(FlexboxHelperTest, BasicRowInversed) {
  flexbox_helper::Block block_10_5;
  block_10_5.min_size_x = 10;
  block_10_5.min_size_y = 5;

  flexbox_helper::Global g;
  g.blocks = {
      block_10_5, block_10_5, block_10_5, block_10_5, block_10_5,
  };

  g.size_x = 32;
  g.size_y = 16;
  g.config = FlexboxConfig().Set(FlexboxConfig::Direction::RowInversed);
  flexbox_helper::Compute(g);

  EXPECT_EQ(g.blocks.size(), 5u);

  EXPECT_EQ(g.blocks[0].line, 0);
  EXPECT_EQ(g.blocks[1].line, 0);
  EXPECT_EQ(g.blocks[2].line, 0);
  EXPECT_EQ(g.blocks[3].line, 1);
  EXPECT_EQ(g.blocks[4].line, 1);

  EXPECT_EQ(g.blocks[0].line_position, 0);
  EXPECT_EQ(g.blocks[1].line_position, 1);
  EXPECT_EQ(g.blocks[2].line_position, 2);
  EXPECT_EQ(g.blocks[3].line_position, 0);
  EXPECT_EQ(g.blocks[4].line_position, 1);

  EXPECT_EQ(g.blocks[0].x, 22);
  EXPECT_EQ(g.blocks[0].y, 0);
  EXPECT_EQ(g.blocks[0].dim_x, 10);
  EXPECT_EQ(g.blocks[0].dim_y, 5);

  EXPECT_EQ(g.blocks[1].x, 12);
  EXPECT_EQ(g.blocks[1].y, 0);
  EXPECT_EQ(g.blocks[1].dim_x, 10);
  EXPECT_EQ(g.blocks[1].dim_y, 5);

  EXPECT_EQ(g.blocks[2].x, 2);
  EXPECT_EQ(g.blocks[2].y, 0);
  EXPECT_EQ(g.blocks[2].dim_x, 10);
  EXPECT_EQ(g.blocks[2].dim_y, 5);

  EXPECT_EQ(g.blocks[3].x, 22);
  EXPECT_EQ(g.blocks[3].y, 5);
  EXPECT_EQ(g.blocks[3].dim_x, 10);
  EXPECT_EQ(g.blocks[3].dim_y, 5);

  EXPECT_EQ(g.blocks[4].x, 12);
  EXPECT_EQ(g.blocks[4].y, 5);
  EXPECT_EQ(g.blocks[4].dim_x, 10);
  EXPECT_EQ(g.blocks[4].dim_y, 5);
}

TEST(FlexboxHelperTest, BasicColumnInversed) {
  flexbox_helper::Block block_10_5;
  block_10_5.min_size_x = 10;
  block_10_5.min_size_y = 5;

  flexbox_helper::Global g;
  g.blocks = {
      block_10_5, block_10_5, block_10_5, block_10_5, block_10_5,
  };

  g.size_x = 32;
  g.size_y = 16;
  g.config = FlexboxConfig().Set(FlexboxConfig::Direction::ColumnInversed);
  flexbox_helper::Compute(g);

  EXPECT_EQ(g.blocks.size(), 5u);

  EXPECT_EQ(g.blocks[0].line, 0);
  EXPECT_EQ(g.blocks[1].line, 0);
  EXPECT_EQ(g.blocks[2].line, 0);
  EXPECT_EQ(g.blocks[3].line, 1);
  EXPECT_EQ(g.blocks[4].line, 1);

  EXPECT_EQ(g.blocks[0].line_position, 0);
  EXPECT_EQ(g.blocks[1].line_position, 1);
  EXPECT_EQ(g.blocks[2].line_position, 2);
  EXPECT_EQ(g.blocks[3].line_position, 0);
  EXPECT_EQ(g.blocks[4].line_position, 1);

  EXPECT_EQ(g.blocks[0].x, 0);
  EXPECT_EQ(g.blocks[0].y, 11);
  EXPECT_EQ(g.blocks[0].dim_x, 10);
  EXPECT_EQ(g.blocks[0].dim_y, 5);

  EXPECT_EQ(g.blocks[1].x, 0);
  EXPECT_EQ(g.blocks[1].y, 6);
  EXPECT_EQ(g.blocks[1].dim_x, 10);
  EXPECT_EQ(g.blocks[1].dim_y, 5);

  EXPECT_EQ(g.blocks[2].x, 0);
  EXPECT_EQ(g.blocks[2].y, 1);
  EXPECT_EQ(g.blocks[2].dim_x, 10);
  EXPECT_EQ(g.blocks[2].dim_y, 5);

  EXPECT_EQ(g.blocks[3].x, 10);
  EXPECT_EQ(g.blocks[3].y, 11);
  EXPECT_EQ(g.blocks[3].dim_x, 10);
  EXPECT_EQ(g.blocks[3].dim_y, 5);

  EXPECT_EQ(g.blocks[4].x, 10);
  EXPECT_EQ(g.blocks[4].y, 6);
  EXPECT_EQ(g.blocks[4].dim_x, 10);
  EXPECT_EQ(g.blocks[4].dim_y, 5);
}

}  // namespace ftxui
// NOLINTEND
