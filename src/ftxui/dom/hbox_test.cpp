#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

using namespace ftxui;
using namespace ftxui;

TEST(HBoxTest, NoFlex_NoFlex_NoFlex) {
  auto root = hbox({
      text(L"012"),
      text(L"abc"),
      text(L"ABC"),
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_NoFlex) {
  auto root = hbox({
      text(L"012") | flex_grow,
      text(L"abc"),
      text(L"ABC"),
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_FlexGrow_NoFlex) {
  auto root = hbox({
      text(L"012"),
      text(L"abc") | flex_grow,
      text(L"ABC"),
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_NoFlex_FlexGrow) {
  auto root = hbox({
      text(L"012"),
      text(L"abc"),
      text(L"ABC") | flex_grow,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_FlexGrow) {
  auto root = hbox({
      text(L"012") | flex_grow,
      text(L"abc"),
      text(L"ABC") | flex_grow,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_FlexGrow_FlexGrow) {
  auto root = hbox({
      text(L"012") | flex_grow,
      text(L"abc") | flex_grow,
      text(L"ABC") | flex_grow,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

// ------

TEST(HBoxTest, FlexShrink_NoFlex_NoFlex) {
  auto root = hbox({
      text(L"012") | flex_shrink,
      text(L"abc"),
      text(L"ABC"),
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_FlexShrink_NoFlex) {
  auto root = hbox({
      text(L"012"),
      text(L"abc") | flex_shrink,
      text(L"ABC"),
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_NoFlex_FlexShrink) {
  auto root = hbox({
      text(L"012"),
      text(L"abc"),
      text(L"ABC") | flex_shrink,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexShrink_NoFlex_FlexShrink) {
  auto root = hbox({
      text(L"012") | flex_shrink,
      text(L"abc"),
      text(L"ABC") | flex_shrink,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexShrink_FlexShrink_FlexShrink) {
  auto root = hbox({
      text(L"012") | flex_shrink,
      text(L"abc") | flex_shrink,
      text(L"ABC") | flex_shrink,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_FlewShrink) {
  auto root = hbox({
      text(L"012") | flex_grow,
      text(L"abc"),
      text(L"ABC") | flex_shrink,
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
  for (int i = 0; i < expectations.size(); ++i) {
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
