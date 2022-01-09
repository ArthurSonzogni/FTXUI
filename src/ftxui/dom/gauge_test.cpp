#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include <memory>                   // for allocator

#include "ftxui/dom/elements.hpp"   // for gauge
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/color.hpp"   // for ftxui
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

using namespace ftxui;
using namespace ftxui;

TEST(GaugeTest, zero) {
  auto root = gauge(0);
  Screen screen(11, 1);
  Render(screen, root);

  EXPECT_EQ("           ", screen.ToString());
}

TEST(GaugeTest, half) {
  auto root = gauge(0.5);
  Screen screen(11, 1);
  Render(screen, root);

  EXPECT_EQ("█████▍     ", screen.ToString());
  //"  ▏▎▍▌▊▉█";
}

TEST(GaugeTest, one) {
  auto root = gauge(1.0);
  Screen screen(11, 1);
  Render(screen, root);

  EXPECT_EQ("███████████", screen.ToString());
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
