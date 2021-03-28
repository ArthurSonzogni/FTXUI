#include "ftxui/component/input.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

TEST(InputTest, Init) {
  Input input;

  EXPECT_EQ(input.content, L"");
  EXPECT_EQ(input.placeholder, L"");
  EXPECT_EQ(input.cursor_position, 0);
}

TEST(InputTest, Type) {
  Input input;

  input.OnEvent(Event::Character('a'));
  EXPECT_EQ(input.content, L"a");
  EXPECT_EQ(input.cursor_position, 1u);

  input.OnEvent(Event::Character('b'));
  EXPECT_EQ(input.content, L"ab");
  EXPECT_EQ(input.cursor_position, 2u);
}

TEST(InputTest, Arrow) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));

  EXPECT_EQ(input.cursor_position, 3u);

  input.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(input.cursor_position, 2u);

  input.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(input.cursor_position, 1u);

  input.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(input.cursor_position, 0u);

  input.OnEvent(Event::ArrowLeft);
  EXPECT_EQ(input.cursor_position, 0u);

  input.OnEvent(Event::ArrowRight);
  EXPECT_EQ(input.cursor_position, 1u);

  input.OnEvent(Event::ArrowRight);
  EXPECT_EQ(input.cursor_position, 2u);

  input.OnEvent(Event::ArrowRight);
  EXPECT_EQ(input.cursor_position, 3u);

  input.OnEvent(Event::ArrowRight);
  EXPECT_EQ(input.cursor_position, 3u);
}

TEST(InputTest, Insert) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));
  EXPECT_EQ(input.content, L"abc");

  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::Character('-'));
  EXPECT_EQ(input.content, L"a-bc");

  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::Character('-'));
  EXPECT_EQ(input.content, L"a--bc");

  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::Character('-'));
  EXPECT_EQ(input.content, L"-a--bc");
}

TEST(InputTest, Home) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));
  EXPECT_EQ(input.content, L"abc");

  EXPECT_EQ(input.cursor_position, 3u);
  input.OnEvent(Event::Home);
  EXPECT_EQ(input.cursor_position, 0u);

  input.OnEvent(Event::Character('-'));
  EXPECT_EQ(input.content, L"-abc");
}

TEST(InputTest, End) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));

  input.OnEvent(Event::ArrowLeft);
  input.OnEvent(Event::ArrowLeft);

  EXPECT_EQ(input.cursor_position, 1u);
  input.OnEvent(Event::End);
  EXPECT_EQ(input.cursor_position, 3u);
}

TEST(InputTest, Delete) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));
  input.OnEvent(Event::ArrowLeft);

  EXPECT_EQ(input.content, L"abc");
  EXPECT_EQ(input.cursor_position, 2u);

  input.OnEvent(Event::Delete);
  EXPECT_EQ(input.content, L"ab");
  EXPECT_EQ(input.cursor_position, 2u);

  input.OnEvent(Event::Delete);
  EXPECT_EQ(input.content, L"ab");
  EXPECT_EQ(input.cursor_position, 2u);
}

TEST(InputTest, Backspace) {
  Input input;

  input.OnEvent(Event::Character('a'));
  input.OnEvent(Event::Character('b'));
  input.OnEvent(Event::Character('c'));
  input.OnEvent(Event::ArrowLeft);

  EXPECT_EQ(input.content, L"abc");
  EXPECT_EQ(input.cursor_position, 2u);

  input.OnEvent(Event::Backspace);
  EXPECT_EQ(input.content, L"ac");
  EXPECT_EQ(input.cursor_position, 1u);

  input.OnEvent(Event::Backspace);
  EXPECT_EQ(input.content, L"c");
  EXPECT_EQ(input.cursor_position, 0u);

  input.OnEvent(Event::Backspace);
  EXPECT_EQ(input.content, L"c");
  EXPECT_EQ(input.cursor_position, 0u);
}
