// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>
#include <algorithm>  // for remove
#include <cstddef>    // for size_t
#include <string>     // for string, allocator, basic_string
#include <vector>     // for vector

#include "ftxui/dom/elements.hpp"  // for vtext, operator|, vbox, Element, flex_grow, flex_shrink
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {
namespace {

std::string rotate(std::string str) {
  str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
  return str;
}

}  // namespace

TEST(VBoxText, NoFlex_NoFlex_NoFlex) {
  auto root = vbox({
      vtext("012"),
      vtext("abc"),
      vtext("ABC"),
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

TEST(VBoxText, FlexGrow_NoFlex_NoFlex) {
  auto root = vbox({
      vtext("012") | flex_grow,
      vtext("abc"),
      vtext("ABC"),
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

TEST(VBoxText, NoFlex_FlexGrow_NoFlex) {
  auto root = vbox({
      vtext("012"),
      vtext("abc") | flex_grow,
      vtext("ABC"),
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, NoFlex_NoFlex_FlexGrow) {
  auto root = vbox({
      vtext("012"),
      vtext("abc"),
      vtext("ABC") | flex_grow,
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

TEST(VBoxText, FlexGrow_NoFlex_FlexGrow) {
  auto root = vbox({
      vtext("012") | flex_grow,
      vtext("abc"),
      vtext("ABC") | flex_grow,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, FlexGrow_FlexGrow_FlexGrow) {
  auto root = vbox({
      vtext("012") | flex_grow,
      vtext("abc") | flex_grow,
      vtext("ABC") | flex_grow,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

// ------

TEST(VBoxText, FlexShrink_NoFlex_NoFlex) {
  auto root = vbox({
      vtext("012") | flex_shrink,
      vtext("abc"),
      vtext("ABC"),
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, NoFlex_FlexShrink_NoFlex) {
  auto root = vbox({
      vtext("012"),
      vtext("abc") | flex_shrink,
      vtext("ABC"),
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, NoFlex_NoFlex_FlexShrink) {
  auto root = vbox({
      vtext("012"),
      vtext("abc"),
      vtext("ABC") | flex_shrink,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, FlexShrink_NoFlex_FlexShrink) {
  auto root = vbox({
      vtext("012") | flex_shrink,
      vtext("abc"),
      vtext("ABC") | flex_shrink,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, FlexShrink_FlexShrink_FlexShrink) {
  auto root = vbox({
      vtext("012") | flex_shrink,
      vtext("abc") | flex_shrink,
      vtext("ABC") | flex_shrink,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

TEST(VBoxText, FlexGrow_NoFlex_FlewShrink) {
  auto root = vbox({
      vtext("012") | flex_grow,
      vtext("abc"),
      vtext("ABC") | flex_shrink,
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
  for (size_t i = 0; i < expectations.size(); ++i) {
    Screen screen(1, i);
    Render(screen, root);
    EXPECT_EQ(expectations[i], rotate(screen.ToString()));
  }
}

}  // namespace ftxui
// NOLINTEND
