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
// baseline for the collapsed single-CSI form.
std::string OldNonClearResetPosition(int dimy) {
  std::string ss;
  ss += '\r';  // MOVE_LEFT;
  for (int y = 1; y < dimy; ++y) {
    ss += "\x1B[1A";  // MOVE_UP;
  }
  return ss;
}

// Decode the total upward cursor movement encoded by a ResetPosition string:
// the leading '\r' moves to column 0, and the cursor-up CSIs (single "\x1B[1A"
// moves or one parameterized "\x1B[<n>A") sum to the number of rows moved up.
// This lets us compare the collapsed single-CSI form to the per-row walk by
// terminal *effect* rather than by exact bytes.
int RowsMovedUp(const std::string& s) {
  int total = 0;
  size_t i = 0;
  while ((i = s.find("\x1B[", i)) != std::string::npos) {
    i += 2;  // Skip the CSI introducer.
    int n = 0;
    bool has_digits = false;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
      n = n * 10 + (s[i] - '0');
      has_digits = true;
      ++i;
    }
    if (i < s.size() && s[i] == 'A') {  // Cursor-up final byte.
      total += has_digits ? n : 1;     // No parameter defaults to 1.
    }
  }
  return total;
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

// The collapsed single-CSI form moves the cursor to exactly the same place as
// the old per-row walk-up, for every height. The byte sequences differ (that is
// the whole point of the optimization), so equivalence is checked by terminal
// *effect*: both forms move the cursor up by the same number of rows.
TEST(ScreenTest, ResetPositionNonClearEquivalentToPerRowWalk) {
  for (int dimy : {1, 2, 3, 10, 24, 50, 100}) {
    Screen screen(10, dimy);
    const std::string new_form = screen.ResetPosition(false);
    EXPECT_EQ(RowsMovedUp(new_form), RowsMovedUp(OldNonClearResetPosition(dimy)))
        << "dimy=" << dimy;
    EXPECT_EQ(RowsMovedUp(new_form), dimy - 1) << "dimy=" << dimy;
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
