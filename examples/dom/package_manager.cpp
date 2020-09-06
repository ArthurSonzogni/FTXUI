#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include <iostream>
#include <list>
#include <thread>
#include <vector>

/// @example examples/dom/package_manage.cpp

int main(int argc, const char* argv[]) {
  using namespace ftxui;

  struct Task {
    std::wstring name;
    int number_of_threads;
    int downloaded;
    int size;
  };

  std::list<Task> remaining_tasks = {
      {L"contact server       ", 10, 0, 6 * 25},
      {L"download index.html  ", 10, 0, 9 * 25},
      {L"download script.js   ", 1, 0, 3 * 25},
      {L"download style.js    ", 1, 0, 4 * 25},
      {L"download image.png   ", 1, 0, 5 * 25},
      {L"download big_1.png   ", 1, 0, 30 * 25},
      {L"download icon_1.png  ", 1, 0, 7 * 25},
      {L"download icon_2.png  ", 1, 0, 8 * 25},
      {L"download big_2.png   ", 1, 0, 30 * 25},
      {L"download small_1.png ", 1, 0, 10 * 25},
      {L"download small_2.png ", 1, 0, 11 * 25},
      {L"download small_3.png ", 1, 0, 12 * 25},
  };

  std::list<Task> displayed_task;

  int remaining_threads = 12;

  int nb_queued = remaining_tasks.size();
  int nb_active = 0;
  int nb_done = 0;

  auto to_text = [](int number) {
    return text(to_wstring(number)) | size(WIDTH, EQUAL, 3);
  };

  auto renderTask = [&](const Task& task) {
    auto style = (task.downloaded == task.size) ? dim : bold;
    return hbox({
        text(task.name) | style,
        separator(),
        to_text(task.downloaded),
        text(L"/"),
        to_text(task.size),
        separator(),
        gauge(task.downloaded / float(task.size)),
    });
  };

  auto renderSummary = [&]() {
    auto summary = vbox({
        hbox({
            text(L"- done:   "),
            to_text(nb_done) | bold,
        }) | color(Color::Green),
        hbox({
            text(L"- active: "),
            to_text(nb_active) | bold,
        }) | color(Color::RedLight),
        hbox({
            text(L"- queue:  "),
            to_text(nb_queued) | bold,
        }) | color(Color::Red),
    });

    return window(text(L" Summary "), summary);
  };

  auto render = [&]() {
    std::vector<Element> entries;
    for (auto& task : displayed_task)
      entries.push_back(renderTask(task));

    return vbox({
        // List of tasks.
        window(text(L" Task "), vbox(std::move(entries))),

        // Summary.
        hbox({
            renderSummary(),
            filler(),
        }),
    });
  };

  auto updateModel = [&]() {
    for (auto& task : displayed_task) {
      if (task.downloaded != task.size) {
        task.downloaded++;
      } else if (task.number_of_threads) {
        remaining_threads += task.number_of_threads;
        task.number_of_threads = 0;
        nb_active--;
        nb_done++;
      }
    }

    if (remaining_tasks.size() &&
        remaining_tasks.front().number_of_threads <= remaining_threads) {
      remaining_threads -= remaining_tasks.front().number_of_threads;
      displayed_task.push_back(remaining_tasks.front());
      remaining_tasks.pop_front();
      nb_queued--;
      nb_active++;
    }
  };

  std::string reset_position;
  for (;;) {
    // Draw.
    auto document = render();
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    std::cout << reset_position << screen.ToString() << std::flush;
    reset_position = screen.ResetPosition();

    // Simulate time.
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.01s);

    // Exit
    if (nb_active + nb_queued == 0)
      break;

    // Update the model for the next frame.
    updateModel();
  }
  std::cout << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
