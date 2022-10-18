#include <gtest/gtest.h>
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Input
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Delete, Event::End, Event::Home
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Button, Mouse::Left, Mouse::Motion, Mouse::Pressed
#include "ftxui/dom/elements.hpp"   // for Fit
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Fixed, Screen, Pixel
#include "ftxui/util/ref.hpp"       // for Ref

namespace ftxui {

TEST(InputTest, Init) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  Component input = Input(&content, &placeholder, &option);

  EXPECT_EQ(option.cursor_position(), 0);
}

TEST(InputTest, Type) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  Component input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character("a"));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_position(), 2u);

  auto document = input->Render();
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "a");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "b");
}

TEST(InputTest, TypePassword) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  option.password = true;
  Component input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_position(), 2u);

  auto document = input->Render();
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "•");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "•");
}

TEST(InputTest, Arrow) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));

  EXPECT_EQ(option.cursor_position(), 3u);

  input->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::ArrowLeft);
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_position(), 3u);

  input->OnEvent(Event::ArrowRight);
  EXPECT_EQ(option.cursor_position(), 3u);
}

TEST(InputTest, Insert) {
  std::string content;
  std::string placeholder;
  Component input = Input(&content, &placeholder);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "abc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "a-bc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "a--bc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "-a--bc");
}

TEST(InputTest, Home) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "abc");

  EXPECT_EQ(option.cursor_position(), 3u);
  input->OnEvent(Event::Home);
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, "-abc");
}

TEST(InputTest, End) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);

  EXPECT_EQ(option.cursor_position(), 1u);
  input->OnEvent(Event::End);
  EXPECT_EQ(option.cursor_position(), 3u);
}

TEST(InputTest, Delete) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  input->OnEvent(Event::ArrowLeft);

  EXPECT_EQ(content, "abc");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Delete);
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Delete);
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(option.cursor_position(), 2u);
}

TEST(InputTest, Backspace) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  input->OnEvent(Event::ArrowLeft);

  EXPECT_EQ(content, "abc");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "ac");
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "c");
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, "c");
  EXPECT_EQ(option.cursor_position(), 0u);
}

TEST(InputTest, MouseClick) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character("a"));
  input->OnEvent(Event::Character("b"));
  input->OnEvent(Event::Character("c"));
  input->OnEvent(Event::Character("d"));

  EXPECT_EQ(option.cursor_position(), 4u);

  auto render = [&] {
    auto document = input->Render();
    auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(1));
    Render(screen, document);
  };
  render();

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.y = 0;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 2;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 2u);

  mouse.x = 2;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 2u);

  mouse.x = 1;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 1u);

  mouse.x = 3;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 3u);

  mouse.x = 4;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 4u);

  mouse.x = 5;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 4u);
}

TEST(InputTest, MouseClickComplex) {
  std::string content;
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character("测"));
  input->OnEvent(Event::Character("试"));
  input->OnEvent(Event::Character("a⃒"));
  input->OnEvent(Event::Character("ā"));

  EXPECT_EQ(option.cursor_position(), 4u);

  auto render = [&] {
    auto document = input->Render();
    auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(1));
    Render(screen, document);
  };
  render();

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.y = 0;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 0;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 1;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 1;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 2;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 1u);

  mouse.x = 2;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 1u);

  mouse.x = 1;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 0u);

  mouse.x = 4;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 2u);

  mouse.x = 5;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 3u);

  mouse.x = 6;
  input->OnEvent(Event::Mouse("", mouse));
  render();
  EXPECT_EQ(option.cursor_position(), 4u);
}

TEST(InputTest, CtrlArrowLeft) {
  std::string content = "word word 测ord wo测d word";
  //                     0    5    10    15    20
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 22;
  auto input = Input(&content, &placeholder, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 20u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 15u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 10u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 5u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 0u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 0u);
}

TEST(InputTest, CtrlArrowLeft2) {
  std::string content = "   word  word  测ord  wo测d  word   ";
  //                     0  3  6  9 12  15  18 21  24 27 30 33
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 33;
  auto input = Input(&content, &placeholder, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 27u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 21u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 15u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 9u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 3u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 0u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(option.cursor_position(), 0u);
}

TEST(InputTest, CtrlArrowRight) {
  std::string content = "word word 测ord wo测d word";
  //                     0    5    10    15    20
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 2;
  auto input = Input(&content, &placeholder, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 4);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 9);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 14u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 19u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 24u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 24u);
}

TEST(InputTest, CtrlArrowRight2) {
  std::string content = "   word  word  测ord  wo测d  word   ";
  //                     0  3  6  9 12  15  18 21  24 27 30 33
  std::string placeholder;
  auto option = InputOption();
  option.cursor_position = 0;
  auto input = Input(&content, &placeholder, &option);

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 7u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 13u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 19u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 25u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 31u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 34u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(option.cursor_position(), 34u);
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
