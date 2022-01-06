#include <benchmark/benchmark.h>

#include "ftxui/dom/elements.hpp"  // for separator, gauge, operator|, text, Element, blink, inverted, hbox, vbox, border
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui
#include "ftxui/screen/screen.hpp"  // for Screen

using namespace ftxui;

static void BencharkBasic(benchmark::State& state) {
  while (state.KeepRunning()) {
    auto document = vbox({
                        text("Test"),
                        separator(),
                        hbox({
                            gauge(0.9),
                            separator() | blink,
                            gauge(0.5),
                            separator() | inverted,
                            gauge(0.1),
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

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.l
