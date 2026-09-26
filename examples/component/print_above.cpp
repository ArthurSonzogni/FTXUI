// Copyright 2026 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for string

#include "ftxui/component/app.hpp"        // for App
#include "ftxui/component/component.hpp"  // for Input, Renderer, CatchEvent
#include "ftxui/component/event.hpp"      // for Event
#include "ftxui/dom/elements.hpp"  // for text, paragraph, borderStyled, vbox

// A chat-like app. The messages are printed above the input, and scroll into
// the terminal scrollback. Only the input is redrawn.
int main() {
  using namespace ftxui;

  auto app = App::TerminalOutput();

  std::string input_text;
  auto input = Input(&input_text, "Type a message, then press Enter.");

  input |= CatchEvent([&](Event event) {
    if (event != Event::Return) {
      return false;
    }
    if (input_text.empty()) {
      return true;
    }
    app.PrintAbove(vbox({
        paragraph(input_text) | borderStyled(LIGHT, Color::GrayDark),
        paragraph("You said " + std::to_string(input_text.size()) +
                  " characters."),
    }));
    input_text.clear();
    return true;
  });

  auto renderer = Renderer(input, [&] {
    return vbox({
        separator(),
        hbox({text("> "), input->Render()}),
        text("Press Ctrl+C to quit.") | dim,
    });
  });

  app.Loop(renderer);
}
