#include "ftxui/component/terminal_input_parser.hpp"
#include "ftxui/component/receiver.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

// Test char |c| to are trivially converted into |Event::Character(c)|.
TEST(Event, Character) {
  std::vector<char> basic_char;
  for (char c = 'a'; c <= 'z'; ++c)
    basic_char.push_back(c);
  for (char c = 'A'; c <= 'Z'; ++c)
    basic_char.push_back(c);

  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    for (char c : basic_char)
      parser.Add(c);
  }

  Event received;
  for (char c : basic_char) {
    EXPECT_TRUE(event_receiver->Receive(&received));
    EXPECT_TRUE(received.is_character());
    EXPECT_EQ(c, received.character());
  }
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyWithoutWaiting) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
  }

  Event received;
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyNotEnoughWait) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Timeout(49);
  }

  Event received;
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyEnoughWait) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Timeout(50);
  }

  Event received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_EQ(received, Event::Escape);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseLeftClick) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Add('[');
    parser.Add('3');
    parser.Add('2');
    parser.Add(';');
    parser.Add('1');
    parser.Add('2');
    parser.Add(';');
    parser.Add('4');
    parser.Add('2');
    parser.Add('M');
  }

  Event received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(received.is_mouse());
  EXPECT_EQ(Mouse::Left, received.mouse().button);
  EXPECT_EQ(12, received.mouse().x);
  EXPECT_EQ(42, received.mouse().y);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseMiddleClick) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Add('[');
    parser.Add('3');
    parser.Add('3');
    parser.Add(';');
    parser.Add('1');
    parser.Add('2');
    parser.Add(';');
    parser.Add('4');
    parser.Add('2');
    parser.Add('M');
  }

  Event received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(received.is_mouse());
  EXPECT_EQ(Mouse::Middle, received.mouse().button);
  EXPECT_EQ(12, received.mouse().x);
  EXPECT_EQ(42, received.mouse().y);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseRightClick) {
  auto event_receiver = MakeReceiver<Event>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Add('[');
    parser.Add('3');
    parser.Add('4');
    parser.Add(';');
    parser.Add('1');
    parser.Add('2');
    parser.Add(';');
    parser.Add('4');
    parser.Add('2');
    parser.Add('M');
  }

  Event received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(received.is_mouse());
  EXPECT_EQ(Mouse::Right, received.mouse().button);
  EXPECT_EQ(12, received.mouse().x);
  EXPECT_EQ(42, received.mouse().y);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
