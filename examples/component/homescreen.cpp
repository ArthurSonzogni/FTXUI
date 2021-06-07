#include <array>       // for array
#include <chrono>      // for operator""s, chrono_literals
#include <cmath>       // for sin
#include <functional>  // for ref, reference_wrapper, function
#include <memory>      // for allocator, shared_ptr, __shared_ptr_access
#include <string>      // for wstring, basic_string, operator+, to_wstring
#include <thread>      // for sleep_for, thread
#include <utility>     // for move
#include <vector>      // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Checkbox, Renderer, Horizontal, Vertical, Input, Menu, Radiobox, Tab, Toggle
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/event.hpp"           // for Event, Event::Custom
#include "ftxui/component/input.hpp"           // for InputBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, operator|, color, bgcolor, Element, filler, size, vbox, flex, hbox, graph, separator, EQUAL, WIDTH, hcenter, bold, border, window, HEIGHT, Elements, hflow, flex_grow, frame, gauge, LESS_THAN, spinner, dim, GREATER_THAN
#include "ftxui/screen/color.hpp"  // for Color, Color::BlueLight, Color::RedLight, Color::Black, Color::Blue, Color::Cyan, Color::CyanLight, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::White, Color::Yellow, Color::YellowLight, Color::Default

using namespace ftxui;

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  int shift = 0;

  class Graph {
   public:
    Graph(int* shift) : shift_(shift) {}
    std::vector<int> operator()(int width, int height) {
      std::vector<int> output(width);
      for (int i = 0; i < width; ++i) {
        float v = 0;
        v += 0.1f * sin((i + *shift_) * 0.1f);
        v += 0.2f * sin((i + *shift_ + 10) * 0.15f);
        v += 0.1f * sin((i + *shift_) * 0.03f);
        v *= height;
        v += 0.5f * height;
        output[i] = (int)v;
      }
      return output;
    }
    int* shift_;
  };

  Graph my_graph(&shift);
  auto htop = Renderer([&] {
    auto frequency = vbox({
        text(L"Frequency [Mhz]") | hcenter,
        hbox({
            vbox({
                text(L"2400 "),
                filler(),
                text(L"1200 "),
                filler(),
                text(L"0% "),
            }),
            graph(std::ref(my_graph)) | flex,
        }) | flex,
    });

    auto utilization = vbox({
        text(L"Utilization [%]") | hcenter,
        hbox({
            vbox({
                text(L"100 "),
                filler(),
                text(L"50 "),
                filler(),
                text(L"0 "),
            }),
            graph(std::ref(my_graph)) | color(Color::RedLight),
        }) | flex,
    });

    auto ram = vbox({
        text(L"Ram [Mo]") | hcenter,
        hbox({
            vbox({
                text(L"8192"),
                filler(),
                text(L"4096 "),
                filler(),
                text(L"0 "),
            }),
            graph(std::ref(my_graph)) | color(Color::BlueLight),
        }) | flex,
    });

    return hbox({
               vbox({
                   frequency | flex,
                   separator(),
                   utilization | flex,
               }) | flex,
               separator(),
               ram | flex,
           }) |
           flex | border;
  });

  const std::vector<std::wstring> compiler_entries = {
      L"gcc",
      L"clang",
      L"emcc",
      L"game_maker",
      L"Ada compilers",
      L"ALGOL 60 compilers",
      L"ALGOL 68 compilers",
      L"Assemblers (Intel *86)",
      L"Assemblers (Motorola 68*)",
      L"Assemblers (Zilog Z80)",
      L"Assemblers (other)",
      L"BASIC Compilers",
      L"BASIC interpreters",
      L"Batch compilers",
      L"C compilers",
      L"Source-to-source compilers",
      L"C++ compilers",
      L"C# compilers",
      L"COBOL compilers",
      L"Common Lisp compilers",
      L"D compilers",
      L"DIBOL/DBL compilers",
      L"ECMAScript interpreters",
      L"Eiffel compilers",
      L"Fortran compilers",
      L"Go compilers",
      L"Haskell compilers",
      L"Java compilers",
      L"Pascal compilers",
      L"Perl Interpreters",
      L"PHP compilers",
      L"PL/I compilers",
      L"Python compilers",
      L"Scheme compilers and interpreters",
      L"Smalltalk compilers",
      L"Tcl Interpreters",
      L"VMS Interpreters",
      L"Rexx Interpreters",
      L"CLI compilers",
  };

  int compiler_selected = 0;
  Component compiler = Radiobox(&compiler_entries, &compiler_selected);

  std::array<std::wstring, 4> options_label = {
      L"-Wall",
      L"-Werror",
      L"-lpthread",
      L"-O3",
  };
  std::array<bool, 4> options_state = {
      false,
      false,
      false,
      false,
  };
  std::wstring input_add_content;
  Component input_add = Input(&input_add_content, "input files");

  std::vector<std::wstring> input_entries;
  int input_selected = 0;
  Component input = Menu(&input_entries, &input_selected);

  std::wstring executable_content_ = L"";
  Component executable_ = Input(&executable_content_, "executable");

  Component flags = Container::Vertical({
      Checkbox(&options_label[0], &options_state[0]),
      Checkbox(&options_label[1], &options_state[1]),
      Checkbox(&options_label[2], &options_state[2]),
      Checkbox(&options_label[3], &options_state[3]),
  });

  auto compiler_component = Container::Horizontal({
      compiler,
      flags,
      Container::Vertical({
          executable_,
          Container::Horizontal({
              input_add,
              input,
          }),
      }),
  });

  InputBase::From(input_add)->on_enter = [&] {
    input_entries.push_back(input_add_content);
    input_add_content = L"";
  };

  auto render_command = [&] {
    Elements line;
    // Compiler
    line.push_back(text(compiler_entries[compiler_selected]) | bold);
    // flags
    for (int i = 0; i < 4; ++i) {
      if (options_state[i]) {
        line.push_back(text(L" "));
        line.push_back(text(options_label[i]) | dim);
      }
    }
    // Executable
    if (!executable_content_.empty()) {
      line.push_back(text(L" -o ") | bold);
      line.push_back(text(executable_content_) | color(Color::BlueLight) |
                     bold);
    }
    // Input
    for (auto& it : input_entries) {
      line.push_back(text(L" " + it) | color(Color::RedLight));
    }
    return line;
  };

  auto compiler_renderer = Renderer(compiler_component, [&] {
    auto compiler_win = window(text(L"Compiler"), compiler->Render() | frame);
    auto flags_win = window(text(L"Flags"), flags->Render());
    auto executable_win = window(text(L"Executable:"), executable_->Render());
    auto input_win =
        window(text(L"Input"),
               hbox({
                   vbox({
                       hbox({
                           text(L"Add: "),
                           input_add->Render(),
                       }) | size(WIDTH, EQUAL, 20) |
                           size(HEIGHT, EQUAL, 1),
                       filler(),
                   }),
                   separator(),
                   input->Render() | frame | size(HEIGHT, EQUAL, 3) | flex,
               }));
    return vbox({
               hbox({
                   compiler_win | size(HEIGHT, LESS_THAN, 6),
                   flags_win,
                   vbox({
                       executable_win | size(WIDTH, EQUAL, 20),
                       input_win | size(WIDTH, EQUAL, 60),
                   }),
                   filler(),
               }),
               hflow(render_command()) | flex_grow,
           }) |
           flex_grow | border;
  });

  auto spinner_tab_renderer = Renderer([&] {
    Elements entries;
    for (int i = 0; i < 22; ++i) {
      entries.push_back(spinner(i, shift / 2) | bold |
                        size(WIDTH, GREATER_THAN, 2) | border);
    }
    return hflow(std::move(entries)) | border;
  });

  auto color_tab_renderer = Renderer([] {
    return hbox({
               vbox({
                   color(Color::Default, text(L"Default")),
                   color(Color::Black, text(L"Black")),
                   color(Color::GrayDark, text(L"GrayDark")),
                   color(Color::GrayLight, text(L"GrayLight")),
                   color(Color::White, text(L"White")),
                   color(Color::Blue, text(L"Blue")),
                   color(Color::BlueLight, text(L"BlueLight")),
                   color(Color::Cyan, text(L"Cyan")),
                   color(Color::CyanLight, text(L"CyanLight")),
                   color(Color::Green, text(L"Green")),
                   color(Color::GreenLight, text(L"GreenLight")),
                   color(Color::Magenta, text(L"Magenta")),
                   color(Color::MagentaLight, text(L"MagentaLight")),
                   color(Color::Red, text(L"Red")),
                   color(Color::RedLight, text(L"RedLight")),
                   color(Color::Yellow, text(L"Yellow")),
                   color(Color::YellowLight, text(L"YellowLight")),
               }),
               vbox({
                   bgcolor(Color::Default, text(L"Default")),
                   bgcolor(Color::Black, text(L"Black")),
                   bgcolor(Color::GrayDark, text(L"GrayDark")),
                   bgcolor(Color::GrayLight, text(L"GrayLight")),
                   bgcolor(Color::White, text(L"White")),
                   bgcolor(Color::Blue, text(L"Blue")),
                   bgcolor(Color::BlueLight, text(L"BlueLight")),
                   bgcolor(Color::Cyan, text(L"Cyan")),
                   bgcolor(Color::CyanLight, text(L"CyanLight")),
                   bgcolor(Color::Green, text(L"Green")),
                   bgcolor(Color::GreenLight, text(L"GreenLight")),
                   bgcolor(Color::Magenta, text(L"Magenta")),
                   bgcolor(Color::MagentaLight, text(L"MagentaLight")),
                   bgcolor(Color::Red, text(L"Red")),
                   bgcolor(Color::RedLight, text(L"RedLight")),
                   bgcolor(Color::Yellow, text(L"Yellow")),
                   bgcolor(Color::YellowLight, text(L"YellowLight")),
               }),
           }) |
           hcenter | border;
  });

  auto render_gauge = [&shift](int delta) {
    float progress = (shift + delta) % 1000 / 1000.f;
    return hbox({
        text(std::to_wstring(int(progress * 100)) + L"% ") |
            size(WIDTH, EQUAL, 5),
        gauge(progress),
    });
  };

  auto gauge_component = Renderer([render_gauge] {
    return vbox({
               render_gauge(0) | color(Color::Black),
               render_gauge(100) | color(Color::GrayDark),
               render_gauge(50) | color(Color::GrayLight),
               render_gauge(6894) | color(Color::White),
               separator(),
               render_gauge(6841) | color(Color::Blue),
               render_gauge(9813) | color(Color::BlueLight),
               render_gauge(98765) | color(Color::Cyan),
               render_gauge(98) | color(Color::CyanLight),
               render_gauge(9846) | color(Color::Green),
               render_gauge(1122) | color(Color::GreenLight),
               render_gauge(84) | color(Color::Magenta),
               render_gauge(645) | color(Color::MagentaLight),
               render_gauge(568) | color(Color::Red),
               render_gauge(2222) | color(Color::RedLight),
               render_gauge(220) | color(Color::Yellow),
               render_gauge(348) | color(Color::YellowLight),
           }) |
           border;
  });

  int tab_index = 0;
  std::vector<std::wstring> tab_entries = {
      L"htop", L"color", L"spinner", L"gauge", L"compiler",
  };
  auto tab_selection = Toggle(&tab_entries, &tab_index);
  auto tab_content = Container::Tab(
      {
          htop,
          color_tab_renderer,
          spinner_tab_renderer,
          gauge_component,
          compiler_renderer,
      },
      &tab_index);

  auto main_container = Container::Vertical({
      tab_selection,
      tab_content,
  });

  auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        text(L"FTXUI Demo") | bold | hcenter,
        tab_selection->Render() | hcenter,
        tab_content->Render() | flex,
    });
  });

  std::thread update([&screen, &shift]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      shift++;
      screen.PostEvent(Event::Custom);
    }
  });

  screen.Loop(main_renderer);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
