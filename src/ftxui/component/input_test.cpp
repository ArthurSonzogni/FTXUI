#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Delete, Event::End, Event::Home
#include "ftxui/dom/elements.hpp"     // for Fit
#include "ftxui/dom/node.hpp"         // for Render
#include "ftxui/screen/screen.hpp"    // for Screen, Pixel
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"    // for Test, EXPECT_EQ, TEST

using namespace ftxui;

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

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
