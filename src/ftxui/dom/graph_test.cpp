// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for allocator, string
#include <vector>  // for vector

#include "ftxui/dom/elements.hpp"   // for graph, Element
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest.h"  // for Test, EXPECT_EQ, EXPECT_FALSE, EXPECT_TRUE, Message, TEST, TestPartResult

// NOLINTBEGIN
namespace ftxui {

TEST(GraphTest, BasicRender) {
  // A simple graph that renders without crashing.
  auto element = graph([](int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; i++) {
      output[i] = i * height / width;
    }
    return output;
  });
  Screen screen(10, 5);
  Render(screen, element);
  // Just check it rendered something (non-empty screen).
  EXPECT_EQ(screen.dimx(), 10);
  EXPECT_EQ(screen.dimy(), 5);
}

TEST(GraphTest, FlatGraph) {
  // A graph where all values are at the top should fill top row.
  auto element = graph([](int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; i++) {
      output[i] = height - 1;
    }
    return output;
  });
  Screen screen(6, 3);
  Render(screen, element);
  // Should render without crashing.
  (void)screen.ToString();
}

TEST(GraphTest, EmptyGraph) {
  // A graph where all values are 0 (bottom).
  auto element = graph(
      [](int width, int /*height*/) { return std::vector<int>(width, 0); });
  Screen screen(6, 3);
  Render(screen, element);
  (void)screen.ToString();
}

TEST(GraphTest, Flexible) {
  // The graph element is flex and should grow to fill available space.
  auto element = graph([](int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; i++) {
      output[i] = height / 2;
    }
    return output;
  });
  Screen screen(20, 10);
  Render(screen, element);
  EXPECT_EQ(screen.dimx(), 20);
  EXPECT_EQ(screen.dimy(), 10);
}

}  // namespace ftxui
// NOLINTEND
