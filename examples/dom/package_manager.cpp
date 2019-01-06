#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/screen.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/string.hpp"
#include <list>
#include <vector>

using namespace ftxui;
using namespace ftxui::dom;

int main(int argc, const char *argv[])
{
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
    std::wstring t = to_wstring(number);
    while(t.size() < 3)
      t = L" " + t;
    return text(t);
  };

  auto renderTask = [&](const Task& task) {
    auto style = (task.downloaded == task.size) ? dim : bold;
    return
      hbox(
        text(task.name) | style,
        separator(),
        to_text(task.downloaded),
        text(L"/"),
        to_text(task.size),
        separator(),
        gauge(task.downloaded / float(task.size))
      );
  };

  auto renderSummary = [&]() {
    return
      window(text(L" Summary "),
        vbox(
          hbox(text(L"- done:   "), to_text(nb_done) | bold) | color(Color::Green),
          hbox(text(L"- active: "), to_text(nb_active) | bold ) | color(Color::RedLight),
          hbox(text(L"- queue:  "), to_text(nb_queued) | bold) | color(Color::Red)
        )
      );
  };

  auto render = [&](){
    std::vector<Element> entries;
    for(auto& task : displayed_task)
      entries.push_back(renderTask(task));

    return
      vbox(
        // List of tasks.
        window(text(L" Task "),
          vbox(std::move(entries))
        ),

        // Summary.
        hbox(renderSummary(), filler())
      );
  };

  auto updateModel = [&](){
    for(auto& task : displayed_task) {
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
  for(;;) {

    // Draw.
    auto document = render();
    auto screen = ftxui::Screen::TerminalOutput(document);
    Render(screen, document.get());
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
