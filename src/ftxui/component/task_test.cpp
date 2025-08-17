// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
// Copyright 2024 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/component/task_internal.hpp"

#include <gtest/gtest.h>

#include <thread>  // for sleep_for
#include "ftxui/component/task_runner.hpp"

namespace ftxui::task {

TEST(TaskTest, Basic) {
  std::vector<int> values;

  auto task_1 = [&values] { values.push_back(1); };
  auto task_2 = [&values] { values.push_back(2); };
  auto task_3 = [&values] { values.push_back(3); };

  auto runner = TaskRunner();

  runner.PostTask(task_1);
  runner.PostTask(task_2);
  runner.PostTask(task_3);
  while (true) {
    auto duration = runner.RunUntilIdle();
    if (!duration) {
      break;
    }
    std::this_thread::sleep_for(duration.value());
  }

  EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

TEST(TaskTest, PostedWithinTask) {
  std::vector<int> values;

  auto task_1 = [&values] {
    values.push_back(1);
    auto task_2 = [&values] { values.push_back(5); };
    TaskRunner::Current()->PostTask(std::move(task_2));
    values.push_back(2);
  };

  auto task_2 = [&values] {
    values.push_back(3);
    auto task_2 = [&values] { values.push_back(6); };
    TaskRunner::Current()->PostTask(std::move(task_2));
    values.push_back(4);
  };

  auto runner = TaskRunner();

  runner.PostTask(task_1);
  runner.PostTask(task_2);
  while (true) {
    auto duration = runner.RunUntilIdle();
    if (!duration) {
      break;
    }
    std::this_thread::sleep_for(duration.value());
  }

  EXPECT_EQ(values, (std::vector<int>{1, 2, 3, 4, 5, 6}));
}

TEST(TaskTest, RunDelayedTask) {
  std::vector<int> values;

  auto task_1 = [&values] { values.push_back(1); };
  auto task_2 = [&values] { values.push_back(2); };
  auto task_3 = [&values] { values.push_back(3); };

  auto runner = TaskRunner();

  runner.PostDelayedTask(task_3, std::chrono::milliseconds(300));
  runner.PostDelayedTask(task_1, std::chrono::milliseconds(100));
  runner.PostDelayedTask(task_2, std::chrono::milliseconds(200));
  while (true) {
    auto duration = runner.RunUntilIdle();
    if (!duration) {
      break;
    }
    std::this_thread::sleep_for(duration.value());
  }

  EXPECT_EQ(values, (std::vector<int>{1, 2, 3}));
}

}  // namespace ftxui::task
