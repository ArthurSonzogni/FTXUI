// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <chrono>  // for chrono
#include <cmath>   // for sin
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for string, to_string
#include <utility> // for move
#include <vector>  // for vector

#include "ftxui/component/app.hpp"            // for App
#include "ftxui/component/component.hpp"      // for Renderer, Button, Vertical, Checkbox, Radiobox
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/dom/elements.hpp"             // for operator|, Element, text, vbox, border, window, separator, hbox
#include "ftxui/screen/color.hpp"             // for Color
#include "ftxui/screen/terminal.hpp"          // for Quirks, GetQuirks, SetQuirks

int main() {
  using namespace ftxui;
  auto screen = App::FitComponent();

  auto quirks = Terminal::GetQuirks();

  auto cb_block = Checkbox("Support 8 Unicode block characters", &quirks.block_characters);
  auto cb_cursor = Checkbox("Support cursor hiding", &quirks.cursor_hiding);
  auto cb_ascii = Checkbox("Use ASCII for components", &quirks.component_ascii);

  std::vector<std::string> color_names = {
      "Palette1",
      "Palette16",
      "Palette256",
      "TrueColor",
  };
  auto rb_color = Radiobox(&color_names, (int*)&quirks.color_support);

  auto btn_quit = Button("Quit", screen.ExitLoopClosure(), ButtonOption::Animated());

  bool dummy_checked = true;
  int dummy_selected = 0;
  std::vector<std::string> dummy_options = {"Option"};
  auto dummy_checkbox = Checkbox("Tested", &dummy_checked);
  auto dummy_radiobox = Radiobox(&dummy_options, &dummy_selected);

  auto layout = Container::Vertical({
      cb_block,
      cb_cursor,
      cb_ascii,
      rb_color,
      dummy_checkbox,
      dummy_radiobox,
      btn_quit,
  });

  auto start_time = std::chrono::steady_clock::now();

  auto renderer = Renderer(layout, [&] {
    auto capabilities = screen.TerminalCapabilityNames();
    Elements capability_elements;
    for (const auto& cap : capabilities) {
      capability_elements.push_back(text(" - " + cap));
    }

    auto info_pane = vbox({
        text("Terminal Name: " + screen.TerminalName()),
        text("Terminal Version: " + std::to_string(screen.TerminalVersion())),
        text("Emulator Name: " + screen.TerminalEmulatorName()),
        text("Emulator Version: " + screen.TerminalEmulatorVersion()),
        text("Detected Capabilities:"),
        vbox(std::move(capability_elements)),
    });

    // We only apply the quirks for the demo pane to see the effect.
    // Note: In FTXUI, Terminal settings are global. Applying them here
    // will affect the actual rendering of the returned element tree.
    Terminal::SetQuirks(quirks);

    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = current_time - start_time;
    // Slower sinusoide (0.5f instead of 2.0f)
    // Range 20%-80% -> 0.5f + 0.3f * sin (0.2 to 0.8)
    float gauge_value = 0.5f + 0.3f * std::sin(elapsed.count() * 0.5f);
    screen.RequestAnimationFrame();

    Elements hsv_rows;
    const int saturation = 255;
    for (int value = 0; value < 255; value += 20) {
      Elements line;
      for (int hue = 0; hue < 255; hue += 6) {
        line.push_back(text("▀")                                   //
                       | color(Color::HSV(hue, saturation, value)) //
                       | bgcolor(Color::HSV(hue, saturation, value + 10)));
      }
      hsv_rows.push_back(hbox(std::move(line)));
    }

    auto overrides_pane = vbox({
        cb_block->Render(),
        hbox(text("Verification: "), gauge(gauge_value) | flex),
        separator(),
        cb_cursor->Render(),
        text("(Hides terminal cursor if supported/enabled)") | dim,
        separator(),
        cb_ascii->Render(),
        hbox({
            text("Verification: "),
            dummy_checkbox->Render(),
            text("  "),
            dummy_radiobox->Render(),
        }),
        separator(),
        text("Color Support:"),
        rb_color->Render(),
        text("Verification (HSV Square):"),
        vbox(std::move(hsv_rows)),
    });

    return hbox({
        vbox({
            window(text(" Terminal Info "), info_pane),
            window(text(" Exit "), vbox({
                                       text("Press 'q' or click the button to exit."),
                                       btn_quit->Render(),
                                   })),
        }) | size(WIDTH, EQUAL, 40),
        window(text(" Runtime Quirk Overrides & Visual Verification "), overrides_pane) | flex,
    });
  });

  auto component = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.Exit();
      return true;
    }
    return false;
  });

  screen.Loop(component);
}
