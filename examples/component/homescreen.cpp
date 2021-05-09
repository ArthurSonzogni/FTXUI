#include <array>       // for array
#include <chrono>      // for operator""s, chrono_literals
#include <cmath>       // for sin
#include <functional>  // for ref, reference_wrapper, function
#include <memory>      // for make_shared, __shared_ptr_access
#include <string>  // for allocator, wstring, basic_string, operator+, to_wstring
#include <thread>   // for sleep_for, thread
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Checkbox, Input, Menu, Radiobox, Toggle
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/event.hpp"           // for Event, Event::Custom
#include "ftxui/component/input.hpp"           // for InputBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, operator|, color, bgcolor, Element, filler, size, vbox, flex, hbox, graph, separator, EQUAL, WIDTH, hcenter, bold, border, window, Elements, HEIGHT, hflow, flex_grow, frame, gauge, LESS_THAN, spinner, dim, GREATER_THAN
#include "ftxui/screen/color.hpp"  // for Color, Color::BlueLight, Color::RedLight, Color::Black, Color::Blue, Color::Cyan, Color::CyanLight, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::White, Color::Yellow, Color::YellowLight, Color::Default

using namespace ftxui;

int shift = 0;
class Graph {
 public:
  std::vector<int> operator()(int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i) {
      float v = 0;
      v += 0.1f * sin((i + shift) * 0.1f);
      v += 0.2f * sin((i + shift + 10) * 0.15f);
      v += 0.1f * sin((i + shift) * 0.03f);
      v *= height;
      v += 0.5f * height;
      output[i] = (int)v;
    }
    return output;
  }
};

class HTopComponent : public ComponentBase {
  Graph my_graph;

 public:
  HTopComponent() {}
  ~HTopComponent() override {}

  Element Render() override {
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
  }
};

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

class CompilerComponent : public ComponentBase {
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
  std::wstring input_add_content = L"";
  std::wstring input_add_placeholder = L"input_files";
  Component input_add = Input(&input_add_content, &input_add_placeholder);

  std::vector<std::wstring> input_entries;
  int input_selected = 0;
  Component input = Menu(&input_entries, &input_selected);

  std::wstring executable_content_ = L"";
  std::wstring executable_placeholder_ = L"executable";
  Component executable_ = Input(&executable_content_, &executable_placeholder_);

  Component flags = Container::Vertical({
      Checkbox(&options_label[0], &options_state[0]),
      Checkbox(&options_label[1], &options_state[1]),
      Checkbox(&options_label[2], &options_state[2]),
      Checkbox(&options_label[3], &options_state[3]),
  });

 public:
  ~CompilerComponent() override {}
  CompilerComponent() {
    Add(Container::Horizontal({
        compiler,
        flags,
        Container::Vertical({
            executable_,
            Container::Horizontal({
                input_add,
                input,
            }),
        }),
    }));

    InputBase::From(input_add)->on_enter = [this] {
      input_entries.push_back(input_add_content);
      input_add_content = L"";
    };
  }

  Element Render() override {
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
               hflow(RenderCommandLine()) | flex_grow,
           }) |
           flex_grow | border;
  }

  Elements RenderCommandLine() {
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
      line.push_back(text(L" -O ") | bold);
      line.push_back(text(executable_content_) | color(Color::BlueLight) |
                     bold);
    }
    // Input
    for (auto& it : input_entries) {
      line.push_back(text(L" " + it) | color(Color::RedLight));
    }
    return line;
  }
};

class SpinnerComponent : public ComponentBase {
  Element Render() override {
    Elements entries;
    for (int i = 0; i < 22; ++i) {
      if (i != 0)
        entries.push_back(spinner(i, shift / 2) | bold |
                          size(WIDTH, GREATER_THAN, 2) | border);
    }
    return hflow(std::move(entries)) | border;
  }
};

class ColorComponent : public ComponentBase {
  Element Render() override {
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
  }
};

class GaugeComponent : public ComponentBase {
  Element RenderGauge(int delta) {
    float progress = (shift + delta) % 1000 / 1000.f;
    return hbox({
        text(std::to_wstring(int(progress * 100)) + L"% ") |
            size(WIDTH, EQUAL, 5),
        gauge(progress),
    });
  }
  Element Render() override {
    return vbox({
               RenderGauge(0) | color(Color::Black),
               RenderGauge(100) | color(Color::GrayDark),
               RenderGauge(50) | color(Color::GrayLight),
               RenderGauge(6894) | color(Color::White),
               separator(),
               RenderGauge(6841) | color(Color::Blue),
               RenderGauge(9813) | color(Color::BlueLight),
               RenderGauge(98765) | color(Color::Cyan),
               RenderGauge(98) | color(Color::CyanLight),
               RenderGauge(9846) | color(Color::Green),
               RenderGauge(1122) | color(Color::GreenLight),
               RenderGauge(84) | color(Color::Magenta),
               RenderGauge(645) | color(Color::MagentaLight),
               RenderGauge(568) | color(Color::Red),
               RenderGauge(2222) | color(Color::RedLight),
               RenderGauge(220) | color(Color::Yellow),
               RenderGauge(348) | color(Color::YellowLight),
           }) |
           border;
  };
};

class Tab : public ComponentBase {
 public:
  int tab_index = 0;
  std::vector<std::wstring> tab_entries = {
      L"htop", L"color", L"spinner", L"gauge", L"compiler",
  };
  Component tab_selection = Toggle(&tab_entries, &tab_index);
  Component container =
      Container::Tab(&tab_index,
                     {
                         std::make_shared<HTopComponent>(),
                         std::make_shared<ColorComponent>(),
                         std::make_shared<SpinnerComponent>(),
                         std::make_shared<GaugeComponent>(),
                         std::make_shared<CompilerComponent>(),
                     });

  Component main_container = Container::Vertical({
      tab_selection,
      container,
  });

  Tab() { Add(main_container); }

  Element Render() override {
    return vbox({
        text(L"FTXUI Demo") | bold | hcenter,
        tab_selection->Render() | hcenter,
        container->Render() | flex,
    });
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  std::thread update([&screen]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      shift++;
      screen.PostEvent(Event::Custom);
    }
  });

  Component tab = std::make_shared<Tab>();
  screen.Loop(tab);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
