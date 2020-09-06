#include <cmath>
#include <thread>

#include "ftxui/component/checkbox.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/input.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/radiobox.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/toggle.hpp"
#include "ftxui/screen/string.hpp"

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

class HTopComponent : public Component {
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

class CompilerComponent : public Component {
  Container container = Container::Horizontal();
  RadioBox compiler;
  Container flag = Container::Vertical();
  CheckBox flag_checkbox[4];
  Container subcontainer = Container::Vertical();
  Container input_container = Container::Horizontal();
  Input input_add;
  Menu input;
  Input executable;

 public:
  ~CompilerComponent() override {}
  CompilerComponent() {
    Add(&container);

    // Compiler ----------------------------------------------------------------
    compiler.entries = {
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
    container.Add(&compiler);

    // Flags    ----------------------------------------------------------------
    container.Add(&flag);
    flag_checkbox[0].label = L"-Wall";
    flag_checkbox[1].label = L"-Werror";
    flag_checkbox[2].label = L"-lpthread";
    flag_checkbox[3].label = L"-O3";
    for (auto& c : flag_checkbox)
      flag.Add(&c);

    container.Add(&subcontainer);
    // Executable
    // ----------------------------------------------------------------
    executable.placeholder = L"executable";
    subcontainer.Add(&executable);

    // Input    ----------------------------------------------------------------
    subcontainer.Add(&input_container);

    input_add.placeholder = L"input files";
    input_add.on_enter = [this] {
      input.entries.push_back(input_add.content);
      input_add.content = L"";
    };
    input_container.Add(&input_add);
    input_container.Add(&input);
  }

  Element Render() override {
    auto compiler_win = window(text(L"Compiler"), compiler.Render() | frame);
    auto flags_win = window(text(L"Flags"), flag.Render());
    auto executable_win = window(text(L"Executable:"), executable.Render());
    auto input_win =
        window(text(L"Input"),
               hbox({
                   vbox({
                       hbox({
                           text(L"Add: "),
                           input_add.Render(),
                       }) | size(WIDTH, EQUAL, 20) |
                           size(HEIGHT, EQUAL, 1),
                       filler(),
                   }),
                   separator(),
                   input.Render() | frame | size(HEIGHT, EQUAL, 3) | flex,
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
    line.push_back(text(compiler.entries[compiler.selected]) | bold);
    // flags
    for (auto& it : flag_checkbox) {
      if (it.state) {
        line.push_back(text(L" "));
        line.push_back(text(it.label) | dim);
      }
    }
    // Executable
    if (!executable.content.empty()) {
      line.push_back(text(L" -O ") | bold);
      line.push_back(text(executable.content) | color(Color::BlueLight) | bold);
    }
    // Input
    for (auto& it : input.entries) {
      line.push_back(text(L" " + it) | color(Color::RedLight));
    }
    return line;
  }
};

class SpinnerComponent : public Component {
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

class ColorComponent : public Component {
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

class GaugeComponent : public Component {
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

class Tab : public Component {
 public:
  Container main_container = Container::Vertical();

  Toggle tab_selection;
  Container container = Container::Tab(&tab_selection.selected);

  HTopComponent htop_component;
  ColorComponent color_component;
  SpinnerComponent spinner_component;
  GaugeComponent gauge_component;
  CompilerComponent compiler_component;

  Tab() {
    Add(&main_container);
    main_container.Add(&tab_selection);
    tab_selection.entries = {
        L"htop", L"color", L"spinner", L"gauge", L"compiler",
    };
    main_container.Add(&container);
    container.Add(&htop_component);
    container.Add(&color_component);
    container.Add(&spinner_component);
    container.Add(&gauge_component);
    container.Add(&compiler_component);
  }

  Element Render() override {
    return vbox({
        text(L"FTXUI Demo") | bold | hcenter,
        tab_selection.Render() | hcenter,
        container.Render() | flex,
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

  Tab tab;
  screen.Loop(&tab);

  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
