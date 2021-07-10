#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiResolver, TestFactoryImpl
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for wstring

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Input, Component
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/event.hpp"  // for Event, Event::ArrowLeft, Event::ArrowRight, Event::Backspace, Event::Delete, Event::End, Event::Home
#include "ftxui/util/ref.hpp"         // for Ref
#include "gtest/gtest_pred_impl.h"    // for Test, EXPECT_EQ, TEST

using namespace ftxui;

TEST(InputTest, Init) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
  Component input = Input(&content, &placeholder, &option);

  EXPECT_EQ(option.cursor_position(), 0);
}

TEST(InputTest, Type) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
  Component input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  EXPECT_EQ(content, L"a");
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::Character('b'));
  EXPECT_EQ(content, L"ab");
  EXPECT_EQ(option.cursor_position(), 2u);
}

TEST(InputTest, Arrow) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
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
  std::wstring content;
  std::wstring placeholder;
  Component input = Input(&content, &placeholder);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, L"abc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, L"a-bc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, L"a--bc");

  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::ArrowLeft);
  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, L"-a--bc");
}

TEST(InputTest, Home) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  EXPECT_EQ(content, L"abc");

  EXPECT_EQ(option.cursor_position(), 3u);
  input->OnEvent(Event::Home);
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::Character('-'));
  EXPECT_EQ(content, L"-abc");
}

TEST(InputTest, End) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
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
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  input->OnEvent(Event::ArrowLeft);

  EXPECT_EQ(content, L"abc");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Delete);
  EXPECT_EQ(content, L"ab");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Delete);
  EXPECT_EQ(content, L"ab");
  EXPECT_EQ(option.cursor_position(), 2u);
}

TEST(InputTest, Backspace) {
  std::wstring content;
  std::wstring placeholder;
  auto option = InputOption();
  auto input = Input(&content, &placeholder, &option);

  input->OnEvent(Event::Character('a'));
  input->OnEvent(Event::Character('b'));
  input->OnEvent(Event::Character('c'));
  input->OnEvent(Event::ArrowLeft);

  EXPECT_EQ(content, L"abc");
  EXPECT_EQ(option.cursor_position(), 2u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, L"ac");
  EXPECT_EQ(option.cursor_position(), 1u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, L"c");
  EXPECT_EQ(option.cursor_position(), 0u);

  input->OnEvent(Event::Backspace);
  EXPECT_EQ(content, L"c");
  EXPECT_EQ(option.cursor_position(), 0u);
}

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
