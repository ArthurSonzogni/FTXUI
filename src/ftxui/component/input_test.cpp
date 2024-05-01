// Copyright 2023 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/component.hpp"       // for Input
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowRightCtrl, Event::ArrowLeftCtrl, Event::ArrowLeft, Event::ArrowRight, Event::ArrowDown, Event::ArrowUp, Event::Delete, Event::Backspace, Event::Return, Event::End, Event::Home
#include "ftxui/component/mouse.hpp"  // for Mouse, Mouse::Button, Mouse::Left, Mouse::Motion, Mouse::Pressed
#include "ftxui/dom/elements.hpp"   // for Fit
#include "ftxui/dom/node.hpp"       // for Render
#include "ftxui/screen/screen.hpp"  // for Fixed, Screen, Pixel
#include "ftxui/util/ref.hpp"       // for Ref
#include "gtest/gtest.h"  // for AssertionResult, Message, TestPartResult, EXPECT_EQ, EXPECT_TRUE, Test, EXPECT_FALSE, TEST

namespace ftxui {

TEST(InputTest, Init) {
  std::string content;
  int cursor_position = 0;
  auto option = InputOption();
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });
  EXPECT_EQ(cursor_position, 0);
}

TEST(InputTest, Type) {
  std::string content;
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });

  input->OnEvent(Event::Character("a"));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(cursor_position, 1);

  input->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(cursor_position, 2);

  input->OnEvent(Event::Return);
  EXPECT_EQ(content, "ab\n");
  EXPECT_EQ(cursor_position, 3);

  input->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, "ab\nc");
  EXPECT_EQ(cursor_position, 4);

  auto document = input->Render();

  auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(2));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "a");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "b");
  EXPECT_EQ(screen.PixelAt(0, 1).character, "c");
  EXPECT_EQ(screen.PixelAt(1, 1).character, " ");
}

TEST(InputTest, ArrowLeftRight) {
  std::string content = "abc测测a测\na测\n";
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });
  EXPECT_EQ(cursor_position, 0);

  EXPECT_FALSE(input->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 1);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 1);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 2);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 3);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 6);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 9);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 10);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 13);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 14);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 15);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 18);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 19);

  EXPECT_FALSE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 19);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(cursor_position, 18);
}

TEST(InputTest, ArrowUpDown) {
  std::string content =
      "０\n"
      "００\n"
      "０００\n"
      "０a０\n"
      "００\n"
      "０\n"
      "";
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });

  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 4);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 11);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 21);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 29);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 36);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 40);
  EXPECT_FALSE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 40);

  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 36);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 29);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 21);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 11);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 4);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 0);
  EXPECT_FALSE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_EQ(cursor_position, 3);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 7);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 14);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 24);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 32);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 39);
  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 40);
  EXPECT_FALSE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(cursor_position, 40);

  cursor_position = 39;
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 32);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 24);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 14);
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_EQ(cursor_position, 7);
}

TEST(InputTest, Insert) {
  std::string content;
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });

  EXPECT_TRUE(input->OnEvent(Event::Character('a')));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_EQ(content, "abc");

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Character('-')));
  EXPECT_EQ(content, "a-bc");

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Character('-')));
  EXPECT_EQ(content, "a--bc");

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Character('-')));
  EXPECT_EQ(content, "-a--bc");

  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_EQ(content, "-测a--bc");

  EXPECT_TRUE(input->OnEvent(Event::Character("a")));
  EXPECT_EQ(content, "-测aa--bc");

  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_EQ(content, "-测a测a--bc");
}

TEST(InputTest, Home) {
  std::string content;
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });

  EXPECT_TRUE(input->OnEvent(Event::Character('a')));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_TRUE(input->OnEvent(Event::Return));
  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 9u);

  EXPECT_TRUE(input->OnEvent(Event::Home));
  EXPECT_EQ(cursor_position, 0u);

  EXPECT_TRUE(input->OnEvent(Event::Character('-')));
  EXPECT_EQ(cursor_position, 1u);
  EXPECT_EQ(content, "-abc\n测bc");
}

TEST(InputTest, End) {
  std::string content;
  int cursor_position = 0;
  Component input = Input(&content, {
                                        .cursor_position = &cursor_position,
                                    });

  EXPECT_TRUE(input->OnEvent(Event::Character('a')));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_TRUE(input->OnEvent(Event::Return));
  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_TRUE(input->OnEvent(Event::ArrowUp));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 2u);

  input->OnEvent(Event::End);
  EXPECT_EQ(cursor_position, 9u);
}

TEST(InputTest, Delete) {
  std::string content;
  int cursor_position = 0;
  auto input = Input(&content, {
                                   .cursor_position = &cursor_position,
                               });

  EXPECT_TRUE(input->OnEvent(Event::Character('a')));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_TRUE(input->OnEvent(Event::Return));
  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));

  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 9u);

  EXPECT_FALSE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 9u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 8u);

  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abc\n测b");
  EXPECT_EQ(cursor_position, 8u);

  EXPECT_FALSE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abc\n测b");
  EXPECT_EQ(cursor_position, 8u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abc\nb");
  EXPECT_EQ(cursor_position, 4u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abcb");
  EXPECT_EQ(cursor_position, 3u);

  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "abc");
  EXPECT_EQ(cursor_position, 3u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_TRUE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "");

  EXPECT_FALSE(input->OnEvent(Event::Delete));
  EXPECT_EQ(content, "");
}

TEST(InputTest, Backspace) {
  std::string content;
  int cursor_position = 0;
  auto input = Input(&content, {
                                   .cursor_position = &cursor_position,
                               });

  EXPECT_TRUE(input->OnEvent(Event::Character('a')));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));
  EXPECT_TRUE(input->OnEvent(Event::Return));
  EXPECT_TRUE(input->OnEvent(Event::Character("测")));
  EXPECT_TRUE(input->OnEvent(Event::Character('b')));
  EXPECT_TRUE(input->OnEvent(Event::Character('c')));

  EXPECT_EQ(content, "abc\n测bc");
  EXPECT_EQ(cursor_position, 9u);

  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "abc\n测b");
  EXPECT_EQ(cursor_position, 8u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeft));
  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "abc\nb");
  EXPECT_EQ(cursor_position, 4u);

  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "abcb");
  EXPECT_EQ(cursor_position, 3u);

  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "abb");
  EXPECT_EQ(cursor_position, 2u);

  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(cursor_position, 1u);

  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "b");
  EXPECT_EQ(cursor_position, 0u);

  EXPECT_FALSE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "b");
  EXPECT_EQ(cursor_position, 0u);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRight));
  EXPECT_TRUE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "");
  EXPECT_EQ(cursor_position, 0u);

  EXPECT_FALSE(input->OnEvent(Event::Backspace));
  EXPECT_EQ(content, "");
  EXPECT_EQ(cursor_position, 0u);
}

TEST(InputTest, CtrlArrow) {
  std::string content =
      "word word 测ord wo测d word\n"
      "coucou    coucou coucou\n"
      "coucou coucou coucou\n";
  int cursor_position = 1000;
  auto input = Input(&content, {
                                   .cursor_position = &cursor_position,
                               });

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 67);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 60);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 53);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 46);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 39);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 29);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 24);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 17);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 10);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 5);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_FALSE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 4);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 9);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 16);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 23);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 28);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 35);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 45);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 52);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 59);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 66);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 73);
}

TEST(InputTest, CtrlArrowLeft2) {
  std::string content = "   word  word  测ord  wo测d  word   ";
  int cursor_position = 33;
  auto input = Input(&content, {
                                   .cursor_position = &cursor_position,
                               });

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 31);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 23);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 15);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 9);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 3);

  EXPECT_TRUE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 0);

  EXPECT_FALSE(input->OnEvent(Event::ArrowLeftCtrl));
  EXPECT_EQ(cursor_position, 0);
}

TEST(InputTest, CtrlArrowRight) {
  std::string content =
      "word word 测ord wo测d word\n"
      "coucou dfqdsf jmlkjm";
  int cursor_position = 2;
  auto input = Input(&content, {.cursor_position = &cursor_position});

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 4);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 9);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 16);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 23);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 28);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 35);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 42);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 49);

  EXPECT_FALSE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 49);
}

TEST(InputTest, CtrlArrowRight2) {
  std::string content = "   word  word  测ord  wo测d  word   ";
  int cursor_position = 0;
  auto input = Input(&content, {.cursor_position = &cursor_position});

  // Use CTRL+Left several time
  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 7);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 13);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 21);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 29);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 35);

  EXPECT_TRUE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 38);

  EXPECT_FALSE(input->OnEvent(Event::ArrowRightCtrl));
  EXPECT_EQ(cursor_position, 38);
}

TEST(InputTest, TypePassword) {
  std::string content;
  std::string placeholder;
  int cursor_position = 0;
  Component input = Input(&content, &placeholder,
                          {
                              .password = true,
                              .cursor_position = &cursor_position,
                          });

  input->OnEvent(Event::Character('a'));
  EXPECT_EQ(content, "a");
  EXPECT_EQ(cursor_position, 1u);

  input->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, "ab");
  EXPECT_EQ(cursor_position, 2u);

  auto document = input->Render();
  auto screen = Screen::Create(Dimension::Fit(document));
  Render(screen, document);
  EXPECT_EQ(screen.PixelAt(0, 0).character, "•");
  EXPECT_EQ(screen.PixelAt(1, 0).character, "•");
}

TEST(InputTest, MouseClick) {
  std::string content;
  int cursor_position = 0;
  auto input = Input(&content, {.cursor_position = &cursor_position});

  input->OnEvent(Event::Character("a"));
  input->OnEvent(Event::Character("b"));
  input->OnEvent(Event::Character("c"));
  input->OnEvent(Event::Character("d"));
  input->OnEvent(Event::Return);
  input->OnEvent(Event::Character("a"));
  input->OnEvent(Event::Character("b"));
  input->OnEvent(Event::Character("c"));
  input->OnEvent(Event::Character("d"));
  input->OnEvent(Event::Return);

  EXPECT_EQ(content, "abcd\nabcd\n");
  EXPECT_EQ(cursor_position, 10u);

  auto render = [&] {
    auto document = input->Render();
    auto screen = Screen::Create(Dimension::Fixed(10), Dimension::Fixed(3));
    Render(screen, document);
  };
  render();
  EXPECT_EQ(cursor_position, 10u);

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 0u);

  mouse.x = 2;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 2u);

  mouse.x = 2;
  mouse.y = 0;
  EXPECT_FALSE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 2u);

  mouse.x = 1;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 1u);

  mouse.x = 3;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 3u);

  mouse.x = 4;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 4u);

  mouse.x = 5;
  mouse.y = 0;
  EXPECT_FALSE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 4u);

  mouse.x = 5;
  mouse.y = 1;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 9u);

  mouse.x = 1;
  mouse.y = 1;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 6u);

  mouse.x = 4;
  mouse.y = 2;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 10u);
}

TEST(InputTest, MouseClickComplex) {
  std::string content;
  int cursor_position = 0;
  auto input = Input(&content, {.cursor_position = &cursor_position});

  input->OnEvent(Event::Character("测"));
  input->OnEvent(Event::Character("试"));
  input->OnEvent(Event::Character("a⃒"));
  input->OnEvent(Event::Character("ā"));
  input->OnEvent(Event::Return);
  input->OnEvent(Event::Character("测"));
  input->OnEvent(Event::Character("试"));
  input->OnEvent(Event::Character("a⃒"));
  input->OnEvent(Event::Character("ā"));

  EXPECT_EQ(cursor_position, 27u);

  auto render = [&] {
    auto document = input->Render();
    auto screen = Screen::Create(Dimension::Fixed(100), Dimension::Fixed(4));
    Render(screen, document);
  };
  render();

  Mouse mouse;
  mouse.button = Mouse::Button::Left;
  mouse.motion = Mouse::Motion::Pressed;
  mouse.shift = false;
  mouse.meta = false;
  mouse.control = false;

  mouse.x = 0;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 0);

  mouse.x = 0;
  mouse.y = 1;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 14);

  mouse.x = 1;
  mouse.y = 0;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 3);

  mouse.x = 1;
  mouse.y = 1;
  EXPECT_TRUE(input->OnEvent(Event::Mouse("", mouse)));
  render();
  EXPECT_EQ(cursor_position, 17);
}

TEST(InputTest, OnEnter) {
  std::string content;
  auto option = InputOption();
  bool on_enter_called = false;
  option.on_enter = [&] { on_enter_called = true; };
  Component input = Input(&content, option);

  EXPECT_FALSE(on_enter_called);
  EXPECT_TRUE(input->OnEvent(Event::Return));
  EXPECT_TRUE(on_enter_called);
}

TEST(InputTest, InsertMode) {
  std::string content = "abc\nefg";
  bool insert = true;
  int cursor_position = 1;
  Component input = Input({
      .content = &content,
      .insert = &insert,
      .cursor_position = &cursor_position,
  });

  EXPECT_TRUE(insert);
  EXPECT_TRUE(input->OnEvent(Event::Insert));
  EXPECT_FALSE(insert);

  EXPECT_EQ(content, "abc\nefg");
  EXPECT_TRUE(input->OnEvent(Event::Character('x')));
  EXPECT_EQ(content, "axc\nefg");
  EXPECT_TRUE(input->OnEvent(Event::Character('y')));
  EXPECT_EQ(content, "axy\nefg");
  EXPECT_TRUE(input->OnEvent(Event::Character('z')));
  EXPECT_EQ(content, "axyz\nefg");

  EXPECT_TRUE(input->OnEvent(Event::ArrowDown));
  EXPECT_EQ(content, "axyz\nefg");
  EXPECT_TRUE(input->OnEvent(Event::Character('X')));
  EXPECT_EQ(content, "axyz\nefgX");
}

}  // namespace ftxui
