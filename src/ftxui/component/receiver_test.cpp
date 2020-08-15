#include "ftxui/component/receiver.hpp"

#include <thread>

#include "gtest/gtest.h"

using namespace ftxui;

TEST(Receiver, Basic) {
  auto receiver = MakeReceiver<char>();
  auto sender = receiver->MakeSender();

  sender->Send('a');
  sender->Send('b');
  sender->Send('c');
  sender.reset();

  char a, b, c, d;
  EXPECT_TRUE(receiver->Receive(&a));
  EXPECT_TRUE(receiver->Receive(&b));
  EXPECT_TRUE(receiver->Receive(&c));
  EXPECT_FALSE(receiver->Receive(&d));

  EXPECT_EQ(a, 'a');
  EXPECT_EQ(b, 'b');
  EXPECT_EQ(c, 'c');
}

TEST(Receiver, BasicWithThread) {
  auto r1 = MakeReceiver<char>();
  auto r2 = MakeReceiver<char>();
  auto r3 = MakeReceiver<char>();

  auto s1 = r1->MakeSender();
  auto s2 = r2->MakeSender();
  auto s3 = r3->MakeSender();

  auto s1_bis = r1->MakeSender();

  auto stream = [](Receiver<char> receiver, Sender<char> sender) {
    char c;
    while (receiver->Receive(&c))
      sender->Send(c);
  };

  // Convert data from a different thread.
  auto t12 = std::thread(stream, std::move(r1), std::move(s2));
  auto t23 = std::thread(stream, std::move(r2), std::move(s3));

  // Send some data.
  s1->Send('1');
  s1_bis->Send('2');
  s1->Send('3');
  s1_bis->Send('4');

  // Close the stream.
  s1.reset();
  s1_bis.reset();

  char c;
  EXPECT_TRUE(r3->Receive(&c));
  EXPECT_EQ(c, '1');
  EXPECT_TRUE(r3->Receive(&c));
  EXPECT_EQ(c, '2');
  EXPECT_TRUE(r3->Receive(&c));
  EXPECT_EQ(c, '3');
  EXPECT_TRUE(r3->Receive(&c));
  EXPECT_EQ(c, '4');
  EXPECT_FALSE(r3->Receive(&c));

  // Thread will end at the end of the stream.
  t12.join();
  t23.join();
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
