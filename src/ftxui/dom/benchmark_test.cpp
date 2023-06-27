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
  }
}
BENCHMARK(BencharkBasic)->DenseRange(0, 256, 16);

static void BencharkText(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::string content = "ＨＥＬＬＯ world ";
    for(int i=0; i<state.range(0); ++i) {
      content += content;
    }
    auto document = paragraph(content);
    Screen screen(200,200);
    Render(screen, document);
  }
}
BENCHMARK(BencharkText)->DenseRange(0, 10, 1);

}  // namespace ftxui
// NOLINTEND

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.l
