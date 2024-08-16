// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <benchmark/benchmark.h>

#include "ftxui/dom/elements.hpp"  // for gauge, separator, operator|, text, Element, hbox, vbox, blink, border, inverted
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/screen.hpp"  // for Screen

// NOLINTBEGIN
namespace ftxui {

static void BencharkBasic(benchmark::State& state) {
  while (state.KeepRunning()) {
    auto document = vbox({
                        text("Test"),
                        separator(),
                        hbox({
                            gauge(0.9f),
                            separator() | blink,
                            gauge(0.5f),
                            separator() | inverted,
                            gauge(0.1f),
                            separator(),
                        }),
                        text("Test"),
                    }) |
                    border;
    auto root = gauge(1.0);
    Screen screen(80, state.range(0));
    Render(screen, root);
    screen.ToString();
  }
}
BENCHMARK(BencharkBasic)->DenseRange(0, 256, 16);

static void BencharkText(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::string content = "ＨＥＬＬＯ world ";
    for (int i = 0; i < state.range(0); ++i) {
      content += content;
    }
    auto document = paragraph(content);
    Screen screen(200, 200);
    Render(screen, document);
    screen.ToString();
  }
}
BENCHMARK(BencharkText)->DenseRange(0, 10, 1);

static void BenchmarkStyle(benchmark::State& state) {
  while (state.KeepRunning()) {
    Elements elements;
    for (int i = 0; i < state.range(0); ++i) {
      elements.push_back(vbox({
          text("Test") | bold,
          text("Test") | dim,
          text("Test") | inverted,
          text("Test") | underlined,
          text("Test") | underlinedDouble,
          text("Test") | strikethrough,
          text("Test") | color(Color::Red),
          text("Test") | bgcolor(Color::Red),
          text("Test") | color(Color::RGB(42, 87, 124)),
          text("Test") | bgcolor(Color::RGB(42, 87, 124)),
          text("Test") | color(Color::RGB(42, 87, 124)) |
              bgcolor(Color::RGB(172, 94, 212)),
          text("Test") | blink,
          text("Test") | automerge,
      }));
      elements.push_back(separator());
    }
    auto document = hbox(std::move(elements));
    Screen screen(state.range(1), state.range(1));
    Render(screen, document);
    screen.ToString();
  }
}
BENCHMARK(BenchmarkStyle)
    ->ArgsProduct({
        benchmark::CreateDenseRange(1, 10, 3),     // Number of elements.
        benchmark::CreateDenseRange(10, 200, 20),  // Screen width.
    });

}  // namespace ftxui
// NOLINTEND
