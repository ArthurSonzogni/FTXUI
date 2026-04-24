// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <algorithm>  // for remove
#include <cstddef>    // for size_t
#include <string>     // for allocator, basic_string, string
#include <vector>     // for vector

#include "ftxui/dom/elements.hpp"  // for text, operator|, Element, flex, Elements, flex_grow, flex_shrink, vtext, gridbox, vbox, select, operator|=, border, frame
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

namespace {
std::string rotate(std::string str) {
  str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
  return str;
}

Element cell(const char* t) {
  return text(t) | border;
}
}  // namespace

TEST(GridboxTest, UnfilledRectangular) {
  auto root = gridbox({
      {text("1"), text("2"), text("3"), text("4")},
      {},
      {},
  });
}

TEST(GridboxTest, DifferentSize) {
  auto root = gridbox({
      {cell("1"), cell("22"), cell("333")},
      {cell("4444"), cell("5") | flex, cell("66666")},
      {cell("7"), cell("8"), vbox({cell("9"), cell("10")})},
  });

  Screen screen(20, 12);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭────╮╭──╮╭─────╮   \r\n"
            "│1   ││22││333  │   \r\n"
            "╰────╯╰──╯╰─────╯   \r\n"
            "╭────╮╭──╮╭─────╮   \r\n"
            "│4444││5 ││66666│   \r\n"
            "╰────╯╰──╯╰─────╯   \r\n"
            "╭────╮╭──╮╭─────╮   \r\n"
            "│7   ││8 ││9    │   \r\n"
            "│    ││  │╰─────╯   \r\n"
            "│    ││  │╭─────╮   \r\n"
            "│    ││  ││10   │   \r\n"
            "╰────╯╰──╯╰─────╯   ");
}

TEST(GridboxTest, CenterFlex) {
  auto root = gridbox({
      {cell("1"), cell("2"), cell("3")},
      {cell("4"), cell("5") | flex, cell("6")},
      {cell("7"), cell("8"), cell("9")},
  });

  Screen screen(12, 12);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭─╮╭─╮╭─╮   \r\n"
            "│1││2││3│   \r\n"
            "╰─╯╰─╯╰─╯   \r\n"
            "╭─╮╭─╮╭─╮   \r\n"
            "│4││5││6│   \r\n"
            "╰─╯╰─╯╰─╯   \r\n"
            "╭─╮╭─╮╭─╮   \r\n"
            "│7││8││9│   \r\n"
            "╰─╯╰─╯╰─╯   \r\n"
            "            \r\n"
            "            \r\n"
            "            ");
}

TEST(GridboxTest, CenterFlexCross) {
  auto root = gridbox({
      {cell("1"), cell("2") | flex, cell("3")},
      {cell("4") | flex, cell("5") | flex, cell("6") | flex},
      {cell("7"), cell("8") | flex, cell("9")},
  });

  Screen screen(12, 12);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭─╮╭────╮╭─╮\r\n"
            "│1││2   ││3│\r\n"
            "╰─╯╰────╯╰─╯\r\n"
            "╭─╮╭────╮╭─╮\r\n"
            "│4││5   ││6│\r\n"
            "│ ││    ││ │\r\n"
            "│ ││    ││ │\r\n"
            "│ ││    ││ │\r\n"
            "╰─╯╰────╯╰─╯\r\n"
            "╭─╮╭────╮╭─╮\r\n"
            "│7││8   ││9│\r\n"
            "╰─╯╰────╯╰─╯");
}

TEST(GridboxTest, CenterShrink) {
  auto root = gridbox({
      {cell("111"), cell("222"), cell("333")},
      {cell("444"), cell("555") | flex, cell("444")},
      {cell("777"), cell("888"), cell("999")},
  });

  Screen screen(13, 12);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭───╮╭──╮╭──╮\r\n"
            "│111││22││33│\r\n"
            "╰───╯╰──╯╰──╯\r\n"
            "╭───╮╭──╮╭──╮\r\n"
            "│444││55││44│\r\n"
            "╰───╯╰──╯╰──╯\r\n"
            "╭───╮╭──╮╭──╮\r\n"
            "│777││88││99│\r\n"
            "╰───╯╰──╯╰──╯\r\n"
            "             \r\n"
            "             \r\n"
            "             ");
}

TEST(GridboxTest, CenterShrinkColumn) {
  auto root = gridbox({
      {cell("111"), cell("222") | flex, cell("333")},
      {cell("444"), cell("555") | flex, cell("666")},
      {cell("777"), cell("888") | flex, cell("999")},
  });

  Screen screen(13, 12);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭───╮╭─╮╭───╮\r\n"
            "│111││2││333│\r\n"
            "╰───╯╰─╯╰───╯\r\n"
            "╭───╮╭─╮╭───╮\r\n"
            "│444││5││666│\r\n"
            "╰───╯╰─╯╰───╯\r\n"
            "╭───╮╭─╮╭───╮\r\n"
            "│777││8││999│\r\n"
            "╰───╯╰─╯╰───╯\r\n"
            "             \r\n"
            "             \r\n"
            "             ");
}

TEST(GridboxTest, Horizontal_NoFlex_NoFlex_NoFlex) {
  auto root = gridbox({
      {
          text("012"),
          text("abc"),
          text("ABC"),
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Vertical_NoFlex_NoFlex_NoFlex) {
  auto root = gridbox({
      {vtext("012")},
      {vtext("abc")},
      {vtext("ABC")},
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(GridboxTest, Horizontal_FlexGrow_NoFlex_NoFlex) {
  auto root = gridbox({
      {
          text("012") | flex_grow,
          text("abc"),
          text("ABC"),
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012 abcABC",   //
      "012  abcABC",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Vertical_FlexGrow_NoFlex_NoFlex) {
  auto root = gridbox({
      {vtext("012") | flex_grow},
      {vtext("abc")},
      {vtext("ABC")},
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012 abcABC",   //
      "012  abcABC",  //
  };
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(GridboxTest, Horizontal_NoFlex_FlexGrow_NoFlex) {
  auto root = gridbox({
      {
          text("012"),
          text("abc") | flex_grow,
          text("ABC"),
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012abc ABC",   //
      "012abc  ABC",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_NoFlex_NoFlex_FlexGrow) {
  auto root = gridbox({
      {
          text("012"),
          text("abc"),
          text("ABC") | flex_grow,
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "0aA",          //
      "01aA",         //
      "01abA",        //
      "01abAB",       //
      "012abAB",      //
      "012abcAB",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_FlexGrow_NoFlex_FlexGrow) {
  auto root = gridbox({
      {
          text("012") | flex_grow,
          text("abc"),
          text("ABC") | flex_grow,
      },
  });

  std::vector<std::string> expectations = {
      "",               //
      "0",              //
      "0a",             //
      "0aA",            //
      "01aA",           //
      "01abA",          //
      "01abAB",         //
      "012abAB",        //
      "012abcAB",       //
      "012abcABC",      //
      "012abcABC ",     //
      "012 abcABC ",    //
      "012 abcABC  ",   //
      "012  abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_FlexGrow_FlexGrow_FlexGrow) {
  auto root = gridbox({
      {
          text("012") | flex_grow,
          text("abc") | flex_grow,
          text("ABC") | flex_grow,
      },
  });

  std::vector<std::string> expectations = {
      "",                 //
      "0",                //
      "0a",               //
      "0aA",              //
      "01aA",             //
      "01abA",            //
      "01abAB",           //
      "012abAB",          //
      "012abcAB",         //
      "012abcABC",        //
      "012abcABC ",       //
      "012abc ABC ",      //
      "012 abc ABC ",     //
      "012 abc ABC  ",    //
      "012 abc  ABC  ",   //
      "012  abc  ABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

// ------

TEST(GridboxTest, Horizontal_FlexShrink_NoFlex_NoFlex) {
  auto root = gridbox({
      {
          text("012") | flex_shrink,
          text("abc"),
          text("ABC"),
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "a",            //
      "aA",           //
      "abA",          //
      "abAB",         //
      "abcAB",        //
      "abcABC",       //
      "0abcABC",      //
      "01abcABC",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_NoFlex_FlexShrink_NoFlex) {
  auto root = gridbox({
      {
          text("012"),
          text("abc") | flex_shrink,
          text("ABC"),
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0A",           //
      "01A",          //
      "01AB",         //
      "012AB",        //
      "012ABC",       //
      "012aABC",      //
      "012abABC",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_NoFlex_NoFlex_FlexShrink) {
  auto root = gridbox({
      {
          text("012"),
          text("abc"),
          text("ABC") | flex_shrink,
      },
  });

  std::vector<std::string> expectations = {
      "",             //
      "0",            //
      "0a",           //
      "01a",          //
      "01ab",         //
      "012ab",        //
      "012abc",       //
      "012abcA",      //
      "012abcAB",     //
      "012abcABC",    //
      "012abcABC ",   //
      "012abcABC  ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_FlexShrink_NoFlex_FlexShrink) {
  auto root = gridbox({
      {
          text("012") | flex_shrink,
          text("abc"),
          text("ABC") | flex_shrink,
      },
  });

  std::vector<std::string> expectations = {
      "",            //
      "a",           //
      "ab",          //
      "abc",         //
      "0abc",        //
      "0abcA",       //
      "01abcA",      //
      "01abcAB",     //
      "012abcAB",    //
      "012abcABC",   //
      "012abcABC ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_FlexShrink_FlexShrink_FlexShrink) {
  auto root = gridbox({
      {
          text("012") | flex_shrink,
          text("abc") | flex_shrink,
          text("ABC") | flex_shrink,
      },
  });

  std::vector<std::string> expectations = {
      "",              //
      "0",             //
      "0a",            //
      "0aA",           //
      "01aA",          //
      "01abA",         //
      "01abAB",        //
      "012abAB",       //
      "012abcAB",      //
      "012abcABC",     //
      "012abcABC ",    //
      "012abcABC  ",   //
      "012abcABC   ",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(GridboxTest, Horizontal_FlexGrow_NoFlex_FlewShrink) {
  auto root = gridbox({
      {
          text("012") | flex_grow,
          text("abc"),
          text("ABC") | flex_shrink,
      },
  });

  std::vector<std::string> expectations = {
      "",              //
      "0",             //
      "0a",            //
      "01a",           //
      "01ab",          //
      "012ab",         //
      "012abc",        //
      "012abcA",       //
      "012abcAB",      //
      "012abcABC",     //
      "012 abcABC",    //
      "012  abcABC",   //
      "012   abcABC",  //
  };
  for (int i = 0; i < (int)expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

// Regression test for https://github.com/ArthurSonzogni/FTXUI/issues/259
TEST(GridboxTest, MissingCells) {
  auto root = gridbox({
      {cell("1"), cell("2"), cell("3")},
      {cell("4"), cell("5")},
  });

  Screen screen(20, 7);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭─╮╭─╮╭─╮           \r\n"
            "│1││2││3│           \r\n"
            "╰─╯╰─╯╰─╯           \r\n"
            "╭─╮╭─╮              \r\n"
            "│4││5│              \r\n"
            "╰─╯╰─╯              \r\n"
            "                    ");
}

TEST(GridboxTest, Focus) {
  auto root = gridbox({
      {cell("1"), cell("2"), cell("3"), cell("4")},
      {cell("5"), cell("6"), cell("7"), cell("8")},
      {cell("9"), cell("10"), cell("11"), cell("12")},
      {cell("13"), cell("14") | focus, cell("15"), cell("16")},
      {cell("17"), cell("18"), cell("19"), cell("20")},
  });

  root |= frame;

  Screen screen(4, 3);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "╭──╮\r\n"
            "│14│\r\n"
            "╰──╯");
}

}  // namespace ftxui
// NOLINTEND
