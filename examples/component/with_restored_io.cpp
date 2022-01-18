#include "ftxui/component/component.hpp"           // for Menu
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

int main() {
  using namespace ftxui;

  auto screen = ScreenInteractive::Fullscreen();

  // When pressing this button, "screen.WithRestoredIO" will execute the
  // temporarily uninstall the terminal hook and execute the provided callback
  // function. This allow running the application in a non-interactive mode.
  auto btn_run = Button("Execute with restored IO", screen.WithRestoredIO([] {
        std::system("bash");
    std::cout << "This is a child program using stdin/stdout." << std::endl;
    for (int i = 0; i < 10; ++i) {
      std::cout << "Please enter 10 strings (" << i << "/10)" << std::flush;
      std::string input;
      std::getline(std::cin, input);
    }
        std::system("bash");
  }));

  auto btn_quit = Button("Quit", screen.ExitLoopClosure());

  auto layout = Container::Horizontal({
      btn_run,
      btn_quit,
  });

  auto renderer = Renderer(layout, [&] {
    auto explanation = paragraph(
        "After clicking this button, the ScreenInteractive will be "
        "suspended and access to stdin/stdout will temporarilly be "
        "restore for running a function.");
    auto element = vbox({
        explanation | borderEmpty,
        hbox({
            btn_run->Render(),
            filler(),
            btn_quit->Render(),
        }),
    });

    element = element | borderEmpty | border | size(WIDTH, LESS_THAN, 80) |
              size(HEIGHT, LESS_THAN, 20) | center;
    return element;
  });

  screen.Loop(renderer);
  return EXIT_SUCCESS;
}

// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
