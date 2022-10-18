#include <gtest/gtest.h>
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator
#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/component/component.hpp"          // for Radiobox
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for RadioboxOption
#include "ftxui/component/event.hpp"  // for Event, Event::Return, Event::ArrowDown, Event::ArrowUp, Event::Tab, Event::TabReverse
#include "ftxui/util/ref.hpp"         // for Ref

namespace ftxui {

TEST(RadioboxTest, NavigationArrow) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  // Down + Return
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_FALSE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  // Up + Return
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowUp));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowUp));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_FALSE(radiobox->OnEvent(Event::ArrowUp));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
}

TEST(RadioboxTest, NavigationArrowVim) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  // J + Return
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character('j')));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character('j')));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_FALSE(radiobox->OnEvent(Event::Character('j')));
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  // K + Return
  EXPECT_TRUE(radiobox->OnEvent(Event::Character('k')));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character('k')));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_FALSE(radiobox->OnEvent(Event::Character('k')));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
}

TEST(RadioboxTest, NavigationTab) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  // Tab + Return
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Tab));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Tab));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Tab));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Tab));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Tab));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  // TabReverse + Return
  EXPECT_TRUE(radiobox->OnEvent(Event::TabReverse));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::TabReverse));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::TabReverse));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::TabReverse));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::TabReverse));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
}

TEST(RadioboxTest, NavigationHome) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  selected = 0;
  EXPECT_FALSE(radiobox->OnEvent(Event::Home));
  EXPECT_EQ(selected, 0);

  selected = 1;
  EXPECT_FALSE(radiobox->OnEvent(Event::Home));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_FALSE(radiobox->OnEvent(Event::Home));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);

  selected = 2;
  EXPECT_FALSE(radiobox->OnEvent(Event::Home));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
  EXPECT_FALSE(radiobox->OnEvent(Event::Home));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
}

TEST(RadioboxTest, NavigationPageDown) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3", "4", "5", "6"};
  auto radiobox = Radiobox(&entries, &selected) | yframe;

  Screen screen(1, 3);
  Render(screen, radiobox->Render());

  EXPECT_TRUE(radiobox->OnEvent(Event::PageDown));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  EXPECT_TRUE(radiobox->OnEvent(Event::PageDown));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 4);

  EXPECT_TRUE(radiobox->OnEvent(Event::PageDown));
  EXPECT_EQ(selected, 4);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 5);

  EXPECT_FALSE(radiobox->OnEvent(Event::PageDown));
  EXPECT_EQ(selected, 5);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 5);
}

TEST(RadioboxTest, NavigationPageUp) {
  int selected = 5;
  std::vector<std::string> entries = {"1", "2", "3", "4", "5", "6"};
  auto radiobox = Radiobox(&entries, &selected) | yframe;

  Screen screen(1, 3);
  Render(screen, radiobox->Render());

  EXPECT_TRUE(radiobox->OnEvent(Event::PageUp));
  EXPECT_EQ(selected, 5);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 3);

  EXPECT_TRUE(radiobox->OnEvent(Event::PageUp));
  EXPECT_EQ(selected, 3);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 1);

  EXPECT_TRUE(radiobox->OnEvent(Event::PageUp));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);

  EXPECT_FALSE(radiobox->OnEvent(Event::PageUp));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 0);
}

TEST(RadioboxTest, NavigationEnd) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  selected = 0;
  EXPECT_TRUE(radiobox->OnEvent(Event::End));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_FALSE(radiobox->OnEvent(Event::End));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  selected = 1;
  EXPECT_FALSE(radiobox->OnEvent(Event::End));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
  EXPECT_FALSE(radiobox->OnEvent(Event::End));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);

  selected = 2;
  EXPECT_FALSE(radiobox->OnEvent(Event::End));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));
  EXPECT_EQ(selected, 2);
}

TEST(RadioboxTest, EventSpace) {
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  auto radiobox = Radiobox(&entries, &selected);

  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 0);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character(' ')));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 1);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character(' ')));
  EXPECT_EQ(selected, 2);
  EXPECT_FALSE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_EQ(selected, 2);
  EXPECT_TRUE(radiobox->OnEvent(Event::Character(' ')));
  EXPECT_EQ(selected, 2);
}

TEST(RadioboxTest, RemoveEntries) {
  int focused_entry = 0;
  int selected = 0;
  std::vector<std::string> entries = {"1", "2", "3"};
  RadioboxOption option;
  option.focused_entry = &focused_entry;
  auto radiobox = Radiobox(&entries, &selected, option);

  EXPECT_EQ(selected, 0);
  EXPECT_EQ(focused_entry, 0);

  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_TRUE(radiobox->OnEvent(Event::ArrowDown));
  EXPECT_TRUE(radiobox->OnEvent(Event::Return));

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  entries.resize(2);

  EXPECT_EQ(selected, 2);
  EXPECT_EQ(focused_entry, 2);

  (void)radiobox->Render();

  EXPECT_EQ(selected, 1);
  EXPECT_EQ(focused_entry, 1);
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
