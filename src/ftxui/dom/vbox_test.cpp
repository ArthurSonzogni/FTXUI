#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

using namespace ftxui;
using namespace ftxui;

TEST(VBoxTest, ScreenSmaller1) {
  auto root = vbox(text(L"text_1"), text(L"text_2"));
  Screen screen(6, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1", screen.ToString());
}

TEST(VBoxTest, ScreenFit) {
  auto root = vbox(text(L"text_1"), text(L"text_2"));
  Screen screen(6, 2);
  Render(screen, root.get());

  EXPECT_EQ("text_1\ntext_2", screen.ToString());
}

TEST(VBoxTest, ScreenBigger1) {
  auto root = vbox(text(L"text_1"), text(L"text_2"));
  Screen screen(6, 3);
  Render(screen, root.get());

  EXPECT_EQ("text_1\ntext_2\n      ", screen.ToString());
}
TEST(VBoxTest, ScreenBigger2) {
  auto root = vbox(text(L"text_1"), text(L"text_2"));
  Screen screen(6, 4);
  Render(screen, root.get());

  EXPECT_EQ("text_1\ntext_2\n      \n      ", screen.ToString());
}

TEST(VBoxTest, ScreenSmaller1Flex) {
  auto root = vbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(6, 1);
  Render(screen, root.get());

  EXPECT_EQ("text_1", screen.ToString());
}

TEST(VBoxTest, ScreenFitFlex) {
  auto root = vbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(7, 5);
  Render(screen, root.get());

  EXPECT_EQ(
      "text_1 \n"
      "       \n"
      "       \n"
      "       \n"
      "text_2 ",
      screen.ToString());
}

TEST(VBoxTest, ScreenBigger1Flex) {
  auto root = vbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(6, 3);
  Render(screen, root.get());

  EXPECT_EQ("text_1\n      \ntext_2", screen.ToString());
}
TEST(VBoxTest, ScreenBigger2Flex) {
  auto root = vbox(text(L"text_1"), filler(), text(L"text_2"));
  Screen screen(6, 4);
  Render(screen, root.get());

  EXPECT_EQ("text_1\n      \n      \ntext_2", screen.ToString());
}
