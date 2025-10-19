// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <gtest/gtest.h>  // for Test, TestInfo (ptr only), EXPECT_EQ, Message, TEST, TestPartResult
#include <array>          // for array
#include <cstddef>        // for size_t
#include <queue>
#include <stack>          // for stack
#include <string>         // for allocator, basic_string, string
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "ftxui/dom/elements.hpp"  // for text, operator|, Element, flex_grow, flex_shrink, hbox
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

TEST(HBoxTest, NoFlex_NoFlex_NoFlex) {
  auto root = hbox({
      text("012"),
      text("abc"),
      text("ABC"),
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_NoFlex) {
  auto root = hbox({
      text("012") | flex_grow,
      text("abc"),
      text("ABC"),
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_FlexGrow_NoFlex) {
  auto root = hbox({
      text("012"),
      text("abc") | flex_grow,
      text("ABC"),
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_NoFlex_FlexGrow) {
  auto root = hbox({
      text("012"),
      text("abc"),
      text("ABC") | flex_grow,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_FlexGrow) {
  auto root = hbox({
      text("012") | flex_grow,
      text("abc"),
      text("ABC") | flex_grow,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_FlexGrow_FlexGrow) {
  auto root = hbox({
      text("012") | flex_grow,
      text("abc") | flex_grow,
      text("ABC") | flex_grow,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

// ------

TEST(HBoxTest, FlexShrink_NoFlex_NoFlex) {
  auto root = hbox({
      text("012") | flex_shrink,
      text("abc"),
      text("ABC"),
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_FlexShrink_NoFlex) {
  auto root = hbox({
      text("012"),
      text("abc") | flex_shrink,
      text("ABC"),
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, NoFlex_NoFlex_FlexShrink) {
  auto root = hbox({
      text("012"),
      text("abc"),
      text("ABC") | flex_shrink,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexShrink_NoFlex_FlexShrink) {
  auto root = hbox({
      text("012") | flex_shrink,
      text("abc"),
      text("ABC") | flex_shrink,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexShrink_FlexShrink_FlexShrink) {
  auto root = hbox({
      text("012") | flex_shrink,
      text("abc") | flex_shrink,
      text("ABC") | flex_shrink,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FlexGrow_NoFlex_FlewShrink) {
  auto root = hbox({
      text("012") | flex_grow,
      text("abc"),
      text("ABC") | flex_shrink,
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
    Screen screen(i, 1);
    Render(screen, root);
    EXPECT_EQ(expectations[i], screen.ToString());
  }
}

TEST(HBoxTest, FromElementsContainer) {
  Elements elements_vector{text("0"), text("1")};

  std::array<Element, 2> elements_array{text("0"), text("1")};

  std::deque<Element> elements_deque{text("0"), text("1")};

  std::stack<Element> elements_stack;
  elements_stack.push(text("1"));
  elements_stack.push(text("0"));

  std::queue<Element> elements_queue;
  elements_queue.emplace(text("0"));
  elements_queue.emplace(text("1"));

  const std::vector<Element> collection_hboxes{
      hbox(std::move(elements_vector)), hbox(std::move(elements_array)),
      hbox(std::move(elements_stack)),  hbox(std::move(elements_deque)),
      hbox(std::move(elements_queue)),
  };

  for (const Element& collection_hbox : collection_hboxes) {
    Screen screen(2, 1);
    Render(screen, collection_hbox);
    EXPECT_EQ("01", screen.ToString());
  }

  // Exception: unordered set, which has no guaranteed order.
  std::unordered_set<Element> elements_set{text("0"), text("0")};
  Screen screen(2, 1);
  Render(screen, hbox(elements_set));
  EXPECT_EQ("00", screen.ToString());
};

}  // namespace ftxui
// NOLINTEND
