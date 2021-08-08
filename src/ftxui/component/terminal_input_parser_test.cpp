#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <algorithm>                // for max
#include <memory>                   // for unique_ptr, allocator

#include "ftxui/component/event.hpp"     // for Event, Event::Escape
#include "ftxui/component/receiver.hpp"  // for MakeReceiver, ReceiverImpl
#include "ftxui/component/terminal_input_parser.hpp"
#include "gtest/gtest_pred_impl.h"  // for AssertionResult, Test, EXPECT_EQ, EXPECT_TRUE, EXPECT_FALSE, TEST

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
    EXPECT_EQ(c, received.character()[0]);
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

TEST(Event, UTF8) {
  struct {
    std::vector<unsigned char> input;
    bool valid;
  } kTestCase[] = {
      // Basic characters.
      {{'a'}, true},
      {{'z'}, true},
      {{'A'}, true},
      {{'Z'}, true},
      {{'0'}, true},
      {{'9'}, true},

      // UTF-8 of various size:
      {{0b0100'0001}, true},
      {{0b1100'0010, 0b1000'0000}, true},
      {{0b1110'0010, 0b1000'0000, 0b1000'0000}, true},
      {{0b1111'0010, 0b1000'0000, 0b1000'0000, 0b1000'0000}, true},

      // Overlong UTF-8 encoding:
      {{0b1100'0000, 0b1000'0000}, false},
      {{0b1110'0000, 0b1000'0000, 0b1000'0000}, false},
      {{0b1111'0000, 0b1000'0000, 0b1000'0000, 0b1000'0000}, false},

      // Test limits in between the various legal regions
      // https://unicode.org/versions/corrigendum1.html
      // Limit in between the valid and ina
      // {{0x7F}, true}, => Special sequence.
      {{0x80}, false},
      // ---
      {{0xC1, 0x80}, false},
      {{0xC2, 0x7F}, false},
      {{0xC2, 0x80}, true},
      // ---
      {{0xDF, 0xBF}, true},
      {{0xDF, 0xC0}, false},
      // ---
      {{0xE0, 0x9F, 0x80}, false},
      {{0xE0, 0xA0, 0x7F}, false},
      {{0xE0, 0xA0, 0x80}, true},
      // ---
      {{0xE0, 0xBF, 0xBF}, true},
      // ---
      {{0xE1, 0x7F, 0x80}, false},
      {{0xE1, 0x80, 0x7f}, false},
      {{0xE1, 0x80, 0x80}, true},
      // --
      {{0xEF, 0xBF, 0xBF}, true},
      {{0xEF, 0xC0, 0xBF}, false},
      {{0xEF, 0xBF, 0xC0}, false},
      // --
      {{0xF0, 0x90, 0x80}, false},
      {{0xF0, 0x8F, 0x80, 0x80}, false},
      {{0xF0, 0x90, 0x80, 0x7F}, false},
      {{0xF0, 0x90, 0x80, 0x80}, true},
      // --
      {{0xF1, 0x80, 0x80, 0x80}, true},
      // --
      {{0xF1, 0xBF, 0xBF, 0xBF}, true},
      // --
      {{0xF2, 0x80, 0x80, 0x80}, true},
      // --
      {{0xF4, 0x8F, 0xBF, 0xBF}, true},
      {{0xF4, 0x90, 0xBF, 0xBF}, false},

  };
  for (auto test : kTestCase) {
    auto event_receiver = MakeReceiver<Event>();
    {
      auto parser = TerminalInputParser(event_receiver->MakeSender());
      for (auto input : test.input)
        parser.Add(input);
    }
    Event received;
    if (test.valid) {
      EXPECT_TRUE(event_receiver->Receive(&received));
      EXPECT_TRUE(received.is_character());
    }
    EXPECT_FALSE(event_receiver->Receive(&received));
  }
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
