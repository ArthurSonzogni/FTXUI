#include <memory>  // for allocator, __shared_ptr_access, shared_ptr
#include <string>  // for wstring, operator+, basic_string, char_traits
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Renderer, Horizontal, Tab
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, filler, text, hbox, separator, center, vbox, bold, border, clear_under, dbox, size, GREATER_THAN, HEIGHT

int main(int argc, const char* argv[]) {
  using namespace ftxui;
  auto screen = ScreenInteractive::TerminalOutput();

  // There are two layers. One at depth = 0 and the modal window at depth = 1;
  int depth = 0;

  // The current rating of FTXUI.
  std::wstring rating = L"3/5 stars";

  // At depth=0, two buttons. One for rating FTXUI and one for quitting.
  auto button_rate_ftxui = Button("Rate FTXUI", [&] { depth = 1; });
  auto button_quit = Button("Quit", screen.ExitLoopClosure());

  auto depth_0_container = Container::Horizontal({
      button_rate_ftxui,
      button_quit,
  });
  auto depth_0_renderer = Renderer(depth_0_container, [&] {
    return vbox({
               text(L"Modal dialog example"),
               separator(),
               text(L"☆☆☆ FTXUI:" + rating + L" ☆☆☆") | bold,
               filler(),
               hbox({
                   button_rate_ftxui->Render(),
                   filler(),
                   button_quit->Render(),
               }),
           }) |
           border | size(HEIGHT, GREATER_THAN, 18) | center;
  });

  // At depth=1, The "modal" window.
  std::vector<std::wstring> rating_labels = {
      L"1/5 stars", L"2/5 stars", L"3/5 stars", L"4/5 stars", L"5/5 stars",
  };
  auto on_rating = [&](std::wstring new_rating) {
    rating = new_rating;
    depth = 0;
  };
  auto depth_1_container = Container::Horizontal({
      Button(&rating_labels[0], [&] { on_rating(rating_labels[0]); }),
      Button(&rating_labels[1], [&] { on_rating(rating_labels[1]); }),
      Button(&rating_labels[2], [&] { on_rating(rating_labels[2]); }),
      Button(&rating_labels[3], [&] { on_rating(rating_labels[3]); }),
      Button(&rating_labels[4], [&] { on_rating(rating_labels[4]); }),
  });

  auto depth_1_renderer = Renderer(depth_1_container, [&] {
    return vbox({
               text(L"Do you like FTXUI?"),
               separator(),
               hbox(depth_1_container->Render()),
           }) |
           border;
  });

  auto main_container = Container::Tab(
      {
          depth_0_renderer,
          depth_1_renderer,
      },
      &depth);

  auto main_renderer = Renderer(main_container, [&] {
    Element document = depth_0_renderer->Render();

    if (depth == 1) {
      document = dbox({
          document,
          depth_1_renderer->Render() | clear_under | center,
      });
    }
    return document;
  });

  screen.Loop(main_renderer);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
