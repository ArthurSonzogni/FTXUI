#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <algorithm>                // for max
#include <initializer_list>  // for initializer_list
#include <memory>            // for unique_ptr, allocator
#include <variant>           // for get

#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowLeft, Event::ArrowRight, Event::ArrowUp, Event::Backspace, Event::Custom, Event::Delete, Event::End, Event::Escape, Event::F10, Event::F11, Event::F12, Event::F5, Event::F6, Event::F7, Event::F8, Event::F9, Event::Home, Event::PageDown, Event::PageUp, Event::Tab, Event::TabReverse
#include "ftxui/component/receiver.hpp"  // for MakeReceiver, ReceiverImpl
#include "ftxui/component/terminal_input_parser.hpp"
#include "gtest/gtest_pred_impl.h"  // for AssertionResult, Test, EXPECT_EQ, EXPECT_TRUE, TEST, EXPECT_FALSE

namespace ftxui {

// Test char |c| to are trivially converted into |Event::Character(c)|.
TEST(Event, Character) {
  std::vector<char> basic_char;
  for (char c = 'a'; c <= 'z'; ++c)
    basic_char.push_back(c);
  for (char c = 'A'; c <= 'Z'; ++c)
    basic_char.push_back(c);

  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    for (char c : basic_char)
      parser.Add(c);
  }

  Task received;
  for (char c : basic_char) {
    EXPECT_TRUE(event_receiver->Receive(&received));
    EXPECT_TRUE(std::get<Event>(received).is_character());
    EXPECT_EQ(c, std::get<Event>(received).character()[0]);
  }
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyWithoutWaiting) {
  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
  }

  Task received;
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyNotEnoughWait) {
  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Timeout(49);
  }

  Task received;
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, EscapeKeyEnoughWait) {
  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Timeout(50);
  }

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_EQ(std::get<Event>(received), Event::Escape);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseLeftClickPressed) {
  auto event_receiver = MakeReceiver<Task>();
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

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(std::get<Event>(received).is_mouse());
  EXPECT_EQ(Mouse::Left, std::get<Event>(received).mouse().button);
  EXPECT_EQ(12, std::get<Event>(received).mouse().x);
  EXPECT_EQ(42, std::get<Event>(received).mouse().y);
  EXPECT_EQ(std::get<Event>(received).mouse().motion, Mouse::Pressed);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseLeftClickReleased) {
  auto event_receiver = MakeReceiver<Task>();
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
    parser.Add('m');
  }

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(std::get<Event>(received).is_mouse());
  EXPECT_EQ(Mouse::Left, std::get<Event>(received).mouse().button);
  EXPECT_EQ(12, std::get<Event>(received).mouse().x);
  EXPECT_EQ(42, std::get<Event>(received).mouse().y);
  EXPECT_EQ(std::get<Event>(received).mouse().motion, Mouse::Released);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseReporting) {
  auto event_receiver = MakeReceiver<Task>();
  {
    auto parser = TerminalInputParser(event_receiver->MakeSender());
    parser.Add('\x1B');
    parser.Add('[');
    parser.Add('1');
    parser.Add('2');
    parser.Add(';');
    parser.Add('4');
    parser.Add('2');
    parser.Add('R');
  }

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(std::get<Event>(received).is_cursor_reporting());
  EXPECT_EQ(42, std::get<Event>(received).cursor_x());
  EXPECT_EQ(12, std::get<Event>(received).cursor_y());
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseMiddleClick) {
  auto event_receiver = MakeReceiver<Task>();
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

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(std::get<Event>(received).is_mouse());
  EXPECT_EQ(Mouse::Middle, std::get<Event>(received).mouse().button);
  EXPECT_EQ(12, std::get<Event>(received).mouse().x);
  EXPECT_EQ(42, std::get<Event>(received).mouse().y);
  EXPECT_FALSE(event_receiver->Receive(&received));
}

TEST(Event, MouseRightClick) {
  auto event_receiver = MakeReceiver<Task>();
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

  Task received;
  EXPECT_TRUE(event_receiver->Receive(&received));
  EXPECT_TRUE(std::get<Event>(received).is_mouse());
  EXPECT_EQ(Mouse::Right, std::get<Event>(received).mouse().button);
  EXPECT_EQ(12, std::get<Event>(received).mouse().x);
  EXPECT_EQ(42, std::get<Event>(received).mouse().y);
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
    auto event_receiver = MakeReceiver<Task>();
    {
      auto parser = TerminalInputParser(event_receiver->MakeSender());
      for (auto input : test.input)
        parser.Add(input);
    }
    Task received;
    if (test.valid) {
      EXPECT_TRUE(event_receiver->Receive(&received));
      EXPECT_TRUE(std::get<Event>(received).is_character());
    }
    EXPECT_FALSE(event_receiver->Receive(&received));
  }
}

TEST(Event, NewLine) {
  for (char newline : {'\r', '\n'}) {
    auto event_receiver = MakeReceiver<Task>();
    {
      auto parser = TerminalInputParser(event_receiver->MakeSender());
      parser.Add(newline);
    }
    Task received;
    EXPECT_TRUE(event_receiver->Receive(&received));
    EXPECT_TRUE(std::get<Event>(received) == Event::Return);
  }
}

TEST(Event, Control) {
  struct TestCase {
    char input;
    bool cancel;
  };
  std::vector<TestCase> cases;
  for (int i = 0; i < 32; ++i) {
    if (i == 13 || i == 24 || i == 26 || i == 27)
      continue;
    cases.push_back({char(i), false});
  }
  cases.push_back({char(24), true});
  cases.push_back({char(26), true});
  cases.push_back({char(127), false});

  for (auto test : cases) {
    auto event_receiver = MakeReceiver<Task>();
    {
      auto parser = TerminalInputParser(event_receiver->MakeSender());
      parser.Add(test.input);
    }
    Task received;
    if (test.cancel) {
      EXPECT_FALSE(event_receiver->Receive(&received));
    } else {
      EXPECT_TRUE(event_receiver->Receive(&received));
      EXPECT_EQ(std::get<Event>(received), Event::Special({test.input}));
    }
  }
}

TEST(Event, Special) {
  auto str = [](std::string input) {
    std::vector<unsigned char> output;
    for (auto it : input)
      output.push_back(it);
    return output;
  };
  struct {
    std::vector<unsigned char> input;
    Event expected;
  } kTestCase[] = {
      {str("\x1B[D"), Event::ArrowLeft},
      {str("\x1B[C"), Event::ArrowRight},
      {str("\x1B[A"), Event::ArrowUp},
      {str("\x1B[B"), Event::ArrowDown},
      {{127}, Event::Backspace},
      {str("\x1B[3~"), Event::Delete},
      //{str("\x1B"), Event::Escape},
      {{10}, Event::Return},
      {{9}, Event::Tab},
      {{27, 91, 90}, Event::TabReverse},
      //{str("\x1B[OP"), Event::F1},
      //{str("\x1B[OQ"), Event::F2},
      //{str("\x1B[OR"), Event::F3},
      //{str("\x1B[OS"), Event::F4},
      {str("\x1B[15~"), Event::F5},
      {str("\x1B[17~"), Event::F6},
      {str("\x1B[18~"), Event::F7},
      {str("\x1B[19~"), Event::F8},
      {str("\x1B[20~"), Event::F9},
      {str("\x1B[21~"), Event::F10},
      {str("\x1B[21~"), Event::F11},
      {str("\x1B[24~"), Event::F12},
      {{27, 91, 72}, Event::Home},
      {{27, 91, 70}, Event::End},
      {{27, 91, 53, 126}, Event::PageUp},
      {{27, 91, 54, 126}, Event::PageDown},
      {{0}, Event::Custom},
  };

  for (auto test : kTestCase) {
    auto event_receiver = MakeReceiver<Task>();
    {
      auto parser = TerminalInputParser(event_receiver->MakeSender());
      for (auto input : test.input) {
        parser.Add(input);
      }
    }
    Task received;
    EXPECT_TRUE(event_receiver->Receive(&received));
    EXPECT_EQ(std::get<Event>(received), test.expected);
    EXPECT_FALSE(event_receiver->Receive(&received));
  }
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
