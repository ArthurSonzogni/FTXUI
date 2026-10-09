// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <cctype>   // for isdigit
#include <cstdlib>  // for atoi
#include <random>   // for mt19937
#include <string>
#include <vector>

#include "ftxui/screen/color.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"
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
      total += has_digits ? n : 1;      // No parameter defaults to 1.
    }
  }
  return total;
}
}  // namespace

// The non-clear ResetPosition emits a single parameterized CSI cursor-up.
TEST(ScreenTest, ResetPositionNonClearSingleCSI) {
  for (int dimy : {2, 3, 10, 24, 50, 100}) {
    Screen screen(10, dimy);
    const std::string expected = "\r\x1B[" + std::to_string(dimy - 1) + "A";
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
    EXPECT_EQ(RowsMovedUp(new_form),
              RowsMovedUp(OldNonClearResetPosition(dimy)))
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

// Regression test: negative/zero dimensions do not crash or throw.
TEST(ScreenTest, NegativeAndZeroDimensions) {
  EXPECT_NO_THROW({
    Screen screen(-10, 20);
    EXPECT_EQ(screen.dimx(), 0);
    EXPECT_EQ(screen.dimy(), 20);
  });
  EXPECT_NO_THROW({
    Screen screen(20, -10);
    EXPECT_EQ(screen.dimx(), 20);
    EXPECT_EQ(screen.dimy(), 0);
  });
  EXPECT_NO_THROW({
    Screen screen(0, 0);
    EXPECT_EQ(screen.dimx(), 0);
    EXPECT_EQ(screen.dimy(), 0);
  });
}

// Distinct colors printing identically once degraded to the terminal color
// support don't emit redundant escape sequences.
TEST(ScreenTest, DegradedColorsAreNotRepeated) {
  Screen screen(3, 1);
  screen.PixelAt(0, 0).foreground_color = Color::RGB(1, 2, 3);
  screen.PixelAt(1, 0).foreground_color = Color::RGB(2, 3, 4);
  screen.PixelAt(2, 0).foreground_color = Color::RGB(3, 3, 3);

  Terminal::SetColorSupport(Terminal::Color::Palette256);
  EXPECT_EQ(screen.ToString(), "\x1B[38;5;16m\x1B[49m   \x1B[39m\x1B[49m");

  Terminal::SetColorSupport(Terminal::Color::TrueColor);
  EXPECT_EQ(screen.ToString(),
            "\x1B[38;2;1;2;3m\x1B[49m "
            "\x1B[38;2;2;3;4m\x1B[49m "
            "\x1B[38;2;3;3;3m\x1B[49m "
            "\x1B[39m\x1B[49m");
}

namespace {
// A minimal terminal emulator, replaying the output of a Screen. It records the
// character and the style of every cell.
class VirtualTerminal {
 public:
  VirtualTerminal(int dimx, int dimy)
      : dimx_(dimx), cells_(dimy, std::vector<VirtualCell>(dimx)) {}

  void Write(const std::string& s) {
    size_t i = 0;
    while (i < s.size()) {
      if (s[i] == '\r') {
        x = 0;
        ++i;
      } else if (s[i] == '\n') {
        ++y;
        ++i;
      } else if (s.compare(i, 4, "\x1B]8;") == 0) {  // Hyperlink.
        const size_t url = s.find(';', i + 4) + 1;
        const size_t end = s.find("\x1B\\", url);
        style_.link = s.substr(url, end - url);
        i = end + 2;
      } else if (s.compare(i, 2, "\x1B[") == 0) {  // CSI.
        i += 2;
        const size_t params = i;
        while (std::isdigit(s[i]) || s[i] == ';' || s[i] == '?') {
          ++i;
        }
        Csi(s.substr(params, i - params), s[i]);
        ++i;
      } else {
        size_t end = i;
        while (end < s.size() && s[end] != '\x1B' && s[end] != '\r' &&
               s[end] != '\n') {
          ++end;
        }
        for (const std::string& glyph : Utf8ToGlyphs(s.substr(i, end - i))) {
          Put(glyph);
        }
        i = end;
      }
    }
  }

  // The displayed content, without the second half of fullwidth characters.
  // The style of a cell is appended to it, when not the default one.
  std::string Content() const {
    const std::string default_style = Style().Key();
    std::string out;
    for (const auto& row : cells_) {
      for (const auto& cell : row) {
        out += cell.glyph;
        if (!cell.glyph.empty() && cell.style != default_style) {
          out += "{" + cell.style + "}";
        }
      }
      out += '\n';
    }
    return out;
  }

  int x = 0;
  int y = 0;

 private:
  struct Style {
    bool bold = false;
    bool dim = false;
    bool italic = false;
    bool blink = false;
    bool inverted = false;
    bool strikethrough = false;
    int underline = 0;
    std::string foreground = "39";
    std::string background = "49";
    std::string link;

    std::string Key() const {
      return std::to_string(bold) + std::to_string(dim) +
             std::to_string(italic) + std::to_string(blink) +
             std::to_string(inverted) + std::to_string(strikethrough) +
             std::to_string(underline) + " fg:" + foreground +
             " bg:" + background + " link:" + link;
    }
  };

  struct VirtualCell {
    std::string glyph = " ";  // Empty for the second half of fullwidth ones.
    std::string style = Style().Key();
  };

  void Csi(const std::string& params, char command) {
    const int n = params.empty() ? 1 : std::atoi(params.c_str());
    switch (command) {
      case 'A':
        y -= n;
        return;
      case 'B':
        y += n;
        return;
      case 'C':
        x += n;
        return;
      case 'D':
        x -= n;
        return;
      case 'm':
        Sgr(params);
        return;
      default:
        return;
    }
  }

  void Sgr(const std::string& params) {
    const int code = params.empty() ? 0 : std::atoi(params.c_str());
    // clang-format off
    switch (code) {
      case 0: { std::string link = style_.link; style_ = Style(); style_.link = link; return; }
      case 1: style_.bold = true; return;
      case 2: style_.dim = true; return;
      case 22: style_.bold = style_.dim = false; return;
      case 3: style_.italic = true; return;
      case 23: style_.italic = false; return;
      case 4: style_.underline = 1; return;
      case 21: style_.underline = 2; return;
      case 24: style_.underline = 0; return;
      case 5: style_.blink = true; return;
      case 25: style_.blink = false; return;
      case 7: style_.inverted = true; return;
      case 27: style_.inverted = false; return;
      case 9: style_.strikethrough = true; return;
      case 29: style_.strikethrough = false; return;
      default: break;
    }
    // clang-format on
    if ((code >= 30 && code <= 39) || (code >= 90 && code <= 97)) {
      style_.foreground = params;
    } else if ((code >= 40 && code <= 49) || (code >= 100 && code <= 107)) {
      style_.background = params;
    } else {
      ADD_FAILURE() << "Unexpected SGR: " << params;
    }
  }

  void Put(const std::string& glyph) {
    if (glyph.empty()) {
      return;  // Second half of a fullwidth character.
    }
    const int width = string_width(glyph) == 2 ? 2 : 1;
    for (int i = 0; i < width; ++i) {
      Erase(x + i);
    }
    cells_[y][x] = {glyph, style_.Key()};
    if (width == 2) {
      cells_[y][x + 1] = {"", style_.Key()};
    }
    x += width;
  }

  // Overwriting half of a fullwidth character erases it.
  void Erase(int x) {
    auto& row = cells_[y];
    if (row[x].glyph.empty()) {
      row[x - 1] = VirtualCell();
    } else if (x + 1 < dimx_ && row[x + 1].glyph.empty()) {
      row[x + 1] = VirtualCell();
    }
    row[x] = VirtualCell();
  }

  int dimx_;
  std::vector<std::vector<VirtualCell>> cells_;
  Style style_;
};

void SetText(Screen& screen, int x, int y, const std::string& text) {
  for (const std::string& glyph : Utf8ToGlyphs(text)) {
    screen.CellAt(x++, y).character = glyph;
  }
}

// Draw |before|, then update it to |after|. Check the terminal ends exactly
// like when fully redrawing |after|. Return the update.
std::string ExpectUpdate(const Screen& before, const Screen& after) {
  VirtualTerminal full(after.dimx(), after.dimy());
  full.Write(before.ToString());
  full.Write(before.ResetPosition());
  full.Write(after.ToString());

  std::string update;
  after.ToString(update, before);
  VirtualTerminal diff(after.dimx(), after.dimy());
  diff.Write(before.ToString());
  diff.Write(before.ResetPosition());
  diff.Write(update);

  EXPECT_EQ(diff.Content(), full.Content());
  EXPECT_EQ(diff.x, full.x);
  EXPECT_EQ(diff.y, full.y);
  return update;
}
}  // namespace

TEST(ScreenTest, ToStringDiffUnchanged) {
  auto screen = Screen(80, 24);
  SetText(screen, 0, 0, "Hello world");
  const std::string update = ExpectUpdate(screen, screen);
  EXPECT_LT(update.size(), 20u);
}

TEST(ScreenTest, ToStringDiffOneCell) {
  auto before = Screen(80, 24);
  SetText(before, 0, 10, "Hello world");
  auto after = before;
  SetText(after, 6, 10, "W");
  const std::string update = ExpectUpdate(before, after);
  EXPECT_LT(update.size(), 40u);
  EXPECT_LT(update.size() * 10, after.ToString().size());
}

TEST(ScreenTest, ToStringDiffStyle) {
  auto before = Screen(10, 3);
  SetText(before, 0, 1, "abc");
  auto after = before;
  after.CellAt(1, 1).bold = true;
  const std::string update = ExpectUpdate(before, after);
  EXPECT_NE(update.find("\x1B[1m"), std::string::npos);
}

TEST(ScreenTest, ToStringDiffHyperlink) {
  auto before = Screen(10, 3);
  SetText(before, 0, 1, "abc");
  before.CellAt(1, 1).hyperlink = before.RegisterHyperlink("https://a.com");
  auto after = Screen(10, 3);
  SetText(after, 0, 1, "abc");
  after.CellAt(1, 1).hyperlink = after.RegisterHyperlink("https://b.com");
  const std::string update = ExpectUpdate(before, after);
  EXPECT_NE(update.find("https://b.com"), std::string::npos);
}

TEST(ScreenTest, ToStringDiffFullwidth) {
  const std::vector<std::string> rows = {
      "abcdef",   //
      "a测试ef",  //
      "ab测试f",  //
      "测试abc",  //
      "abc测试",  //
      "a😀cdef",
  };
  for (const auto& before_row : rows) {
    for (const auto& after_row : rows) {
      auto before = Screen(7, 3);
      SetText(before, 0, 1, before_row);
      auto after = Screen(7, 3);
      SetText(after, 0, 1, after_row);
      SCOPED_TRACE(before_row + " -> " + after_row);
      ExpectUpdate(before, after);
    }
  }
}

// A fullwidth character drawn over the second half of another one, e.g. by
// dbox. It is hidden, and must not hide the next cell.
TEST(ScreenTest, ToStringOverlappingFullwidth) {
  auto screen = Screen(4, 1);
  screen.CellAt(0, 0).character = "测";
  screen.CellAt(1, 0).character = "试";
  screen.CellAt(2, 0).character = "";
  screen.CellAt(3, 0).character = "b";
  EXPECT_EQ(screen.ToString(), "测 b");
}

// Several rows change, including the first and the last ones. The styles must
// not leak from one updated span to the next.
TEST(ScreenTest, ToStringDiffSeveralRows) {
  auto before = Screen(10, 8);
  for (int y = 0; y < 8; ++y) {
    SetText(before, 0, y, "line " + std::to_string(y));
  }
  auto after = before;
  SetText(after, 2, 0, "X");
  after.CellAt(2, 0).bold = true;
  after.CellAt(2, 0).foreground_color = Color::Red;
  SetText(after, 5, 3, "Y");
  after.CellAt(5, 3).background_color = Color::Blue;
  SetText(after, 0, 4, "Z");
  SetText(after, 9, 7, "W");
  after.CellAt(9, 7).underlined = true;
  ExpectUpdate(before, after);
}

// Random screens, with random changes.
TEST(ScreenTest, ToStringDiffRandom) {
  const std::vector<std::string> glyphs = {
      "a", "b", " ", "", "测", "试", "\U0001F600", "\u2764\uFE0F",
  };
  const std::vector<Color> colors = {
      Color::Default,
      Color::Red,
      Color::Blue,
      Color::RGB(1, 2, 3),
  };
  std::mt19937 random(1234);  // NOLINT
  auto pick = [&](int n) { return int(random() % unsigned(n)); };

  auto randomize = [&](Screen& screen, int x, int y) {
    Cell& cell = screen.CellAt(x, y);
    cell.character = glyphs[pick(int(glyphs.size()))];
    // A fullwidth character can't be printed on the last column.
    if (x == screen.dimx() - 1 && string_width(cell.character) == 2) {
      cell.character = "c";
    }
    cell.bold = pick(4) == 0;
    cell.underlined = pick(4) == 0;
    cell.foreground_color = colors[pick(int(colors.size()))];
    cell.background_color = colors[pick(int(colors.size()))];
    cell.hyperlink = pick(4) == 0
                         ? screen.RegisterHyperlink(pick(2) ? "https://a.com"
                                                            : "https://b.com")
                         : 0;
  };

  for (int i = 0; i < 300; ++i) {
    auto before = Screen(8, 4);
    for (int y = 0; y < 4; ++y) {
      for (int x = 0; x < 8; ++x) {
        randomize(before, x, y);
      }
    }
    auto after = before;
    const int changes = pick(6);
    for (int j = 0; j < changes; ++j) {
      randomize(after, pick(8), pick(4));
    }
    SCOPED_TRACE("Iteration " + std::to_string(i));
    ExpectUpdate(before, after);
    if (HasFailure()) {
      return;
    }
  }
}

// A fullwidth character, hiding the next cell, is replaced by a narrow one.
TEST(ScreenTest, ToStringDiffFullwidthUncovered) {
  auto before = Screen(4, 3);
  SetText(before, 0, 1, "测");
  before.CellAt(1, 1).character = "Z";
  auto after = before;
  after.CellAt(0, 1).character = "a";
  ExpectUpdate(before, after);
}

TEST(ScreenTest, ToStringDiffResized) {
  auto before = Screen(5, 2);
  auto after = Screen(6, 2);
  SetText(after, 0, 0, "abc");
  std::string update;
  after.ToString(update, before);
  EXPECT_EQ(update, after.ToString());
}

}  // namespace ftxui
