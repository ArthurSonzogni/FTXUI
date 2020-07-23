// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include "ftxui/component/event.hpp"
#include "ftxui/component/receiver.hpp"

#include "gtest/gtest.h"

using namespace ftxui;

namespace {
// Produce a stream of Event from a stream of char.
void CharToEventStream(Receiver<char> receiver, Sender<Event> sender) {
  char c;
  while (receiver->Receive(&c))
    Event::Convert(receiver, sender, c);
}

}  // namespace

// Test char |c| to are trivially converted into |Event::Character(c)|.
TEST(Event, Character) {
  std::vector<char> basic_char;
  for (char c = 'a'; c < 'z'; ++c)
    basic_char.push_back(c);
  for (char c = 'A'; c < 'Z'; ++c)
    basic_char.push_back(c);

  auto char_receiver = MakeReceiver<char>();
  auto char_sender = char_receiver->MakeSender();

  auto event_receiver = MakeReceiver<Event>();
  auto event_sender = event_receiver->MakeSender();

  for (char c : basic_char)
    char_sender->Send(c);
  char_sender.reset();

  CharToEventStream(std::move(char_receiver), std::move(event_sender));

  Event received;
  for (char c : basic_char) {
    EXPECT_TRUE(event_receiver->Receive(&received));
    EXPECT_TRUE(received.is_character());
    EXPECT_EQ(c, received.character());
  }
  EXPECT_FALSE(event_receiver->Receive(&received));
}
