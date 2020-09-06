#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <thread>

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  using namespace std::chrono_literals;

  std::string reset_position;
  for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.002f) {
    std::wstring data_downloaded =
        std::to_wstring(int(percentage * 5000)) + L"/5000";
    auto document = hbox({
        text(L"downloading:"),
        gauge(percentage) | flex,
        text(L" " + data_downloaded),
    });
    auto screen = Screen(100, 1);
    Render(screen, document);
    std::cout << reset_position << screen.ToString() << std::flush;
    reset_position = screen.ResetPosition();

    std::this_thread::sleep_for(0.01s);
  }
  std::cout << std::endl;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
