// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>

#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

namespace ftxui {

namespace {
// The old, pre-optimization non-clear ResetPosition output: a leading '\r'
// followed by one "\x1B[1A" cursor-up per extra row. Used as the reference
// (equivalence) baseline for the collapsed single-CSI form.
std::string OldNonClearResetPosition(int dimy) {
  std::string ss;
  ss += '\r';  // MOVE_LEFT;
  for (int y = 1; y < dimy; ++y) {
    ss += "\x1B[1A";  // MOVE_UP;
  }
  return ss;
}
}  // namespace

// The non-clear ResetPosition emits a single parameterized CSI cursor-up.
TEST(ScreenTest, ResetPositionNonClearSingleCSI) {
  for (int dimy : {2, 3, 10, 24, 50, 100}) {
    Screen screen(10, dimy);
    const std::string expected =
        "\r\x1B[" + std::to_string(dimy - 1) + "A";
    EXPECT_EQ(screen.ResetPosition(false), expected) << "dimy=" << dimy;
  }
}

// A single-row screen needs no cursor-up: only the leading '\r'.
TEST(ScreenTest, ResetPositionNonClearSingleRow) {
  Screen screen(10, 1);
  EXPECT_EQ(screen.ResetPosition(false), "\r");
}

// The collapsed form moves the cursor to exactly the same place as the old
// per-row walk-up, for every height.
TEST(ScreenTest, ResetPositionNonClearEquivalentToPerRowWalk) {
  for (int dimy : {1, 2, 3, 10, 24, 50, 100}) {
    Screen screen(10, dimy);
    EXPECT_EQ(screen.ResetPosition(false), OldNonClearResetPosition(dimy))
        << "dimy=" << dimy;
  }
}

// The new non-clear output is at least 10x smaller in bytes than the old
// per-row form for a 50-row screen (<= 8 bytes vs 197).
TEST(ScreenTest, ResetPositionNonClearByteReduction) {
  Screen screen(10, 50);
  const std::string new_form = screen.ResetPosition(false);
  const std::string old_form = OldNonClearResetPosition(50);

  EXPECT_EQ(old_form.size(), 197u);
  EXPECT_LE(new_form.size(), 8u);
  EXPECT_GE(old_form.size(), new_form.size() * 10);
}

// The clear branch is intentionally left per-row (each row keeps its own
// CLEAR_LINE erase), so it is NOT collapsed into a single CSI.
TEST(ScreenTest, ResetPositionClearIsPerRow) {
  Screen screen(10, 3);
  std::string expected;
  expected += '\r';
  expected += "\x1b[2K";
  for (int y = 1; y < 3; ++y) {
    expected += "\x1B[1A";
    expected += "\x1B[2K";
  }
  EXPECT_EQ(screen.ResetPosition(true), expected);
}

}  // namespace ftxui
