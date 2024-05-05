// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stddef.h>    // for size_t
#include <array>       // for array
#include <atomic>      // for atomic
#include <chrono>      // for operator""s, chrono_literals
#include <cmath>       // for sin
#include <functional>  // for ref, reference_wrapper, function
#include <memory>      // for allocator, shared_ptr, __shared_ptr_access
#include <string>  // for string, basic_string, char_traits, operator+, to_string
#include <thread>   // for sleep_for, thread
#include <utility>  // for move
#include <vector>   // for vector

#include "../dom/color_info_sorted_2d.ipp"  // for ColorInfoSorted2D
#include "ftxui/component/component.hpp"  // for Checkbox, Renderer, Horizontal, Vertical, Input, Menu, Radiobox, ResizableSplitLeft, Tab
#include "ftxui/component/component_base.hpp"  // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for MenuOption, InputOption
#include "ftxui/component/event.hpp"              // for Event, Event::Custom
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, color, operator|, bgcolor, filler, Element, vbox, size, hbox, separator, flex, window, graph, EQUAL, paragraph, WIDTH, hcenter, Elements, bold, vscroll_indicator, HEIGHT, flexbox, hflow, border, frame, flex_grow, gauge, paragraphAlignCenter, paragraphAlignJustify, paragraphAlignLeft, paragraphAlignRight, dim, spinner, LESS_THAN, center, yframe, GREATER_THAN
#include "ftxui/dom/flexbox_config.hpp"  // for FlexboxConfig
#include "ftxui/screen/color.hpp"  // for Color, Color::BlueLight, Color::RedLight, Color::Black, Color::Blue, Color::Cyan, Color::CyanLight, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::White, Color::Yellow, Color::YellowLight, Color::Default, Color::Palette256, ftxui
#include "ftxui/screen/color_info.hpp"  // for ColorInfo
#include "ftxui/screen/terminal.hpp"    // for Size, Dimensions

using namespace ftxui;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  // ---------------------------------------------------------------------------
  // HTOP
  // ---------------------------------------------------------------------------
  int shift = 0;

  auto my_graph = [&shift](int width, int height) {
    std::vector<int> output(width);
    for (int i = 0; i < width; ++i) {
      float v = 0.5f;
      v += 0.1f * sin((i + shift) * 0.1f);
      v += 0.2f * sin((i + shift + 10) * 0.15f);
      v += 0.1f * sin((i + shift) * 0.03f);
      v *= height;
      output[i] = (int)v;
    }
    return output;
  };

  auto htop = Renderer([&] {
    auto frequency = vbox({
        text("Frequency [Mhz]") | hcenter,
        hbox({
            vbox({
                text("2400 "),
                filler(),
                text("1200 "),
                filler(),
                text("0 "),
            }),
            graph(std::ref(my_graph)) | flex,
        }) | flex,
    });

    auto utilization = vbox({
        text("Utilization [%]") | hcenter,
        hbox({
            vbox({
                text("100 "),
                filler(),
                text("50 "),
                filler(),
                text("0 "),
            }),
            graph(std::ref(my_graph)) | color(Color::RedLight),
        }) | flex,
    });

    auto ram = vbox({
        text("Ram [Mo]") | hcenter,
        hbox({
            vbox({
                text("8192"),
                filler(),
                text("4096 "),
                filler(),
                text("0 "),
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
           flex;
  });

  // ---------------------------------------------------------------------------
  // Compiler
  // ---------------------------------------------------------------------------

  const std::vector<std::string> compiler_entries = {
      "gcc",
      "clang",
      "emcc",
      "game_maker",
      "Ada compilers",
      "ALGOL 60 compilers",
      "ALGOL 68 compilers",
      "Assemblers (Intel *86)",
      "Assemblers (Motorola 68*)",
      "Assemblers (Zilog Z80)",
      "Assemblers (other)",
      "BASIC Compilers",
      "BASIC interpreters",
      "Batch compilers",
      "C compilers",
      "Source-to-source compilers",
      "C++ compilers",
      "C# compilers",
      "COBOL compilers",
      "Common Lisp compilers",
      "D compilers",
      "DIBOL/DBL compilers",
      "ECMAScript interpreters",
      "Eiffel compilers",
      "Fortran compilers",
      "Go compilers",
      "Haskell compilers",
      "Java compilers",
      "Pascal compilers",
      "Perl Interpreters",
      "PHP compilers",
      "PL/I compilers",
      "Python compilers",
      "Scheme compilers and interpreters",
      "Smalltalk compilers",
      "Tcl Interpreters",
      "VMS Interpreters",
      "Rexx Interpreters",
      "CLI compilers",
  };

  int compiler_selected = 0;
  Component compiler = Radiobox(&compiler_entries, &compiler_selected);

  std::array<std::string, 8> options_label = {
      "-Wall",
      "-Werror",
      "-lpthread",
      "-O3",
      "-Wabi-tag",
      "-Wno-class-conversion",
      "-Wcomma-subscript",
      "-Wno-conversion-null",
  };
  std::array<bool, 8> options_state = {
      false, false, false, false, false, false, false, false,
  };

  std::vector<std::string> input_entries;
  int input_selected = 0;
  Component input = Menu(&input_entries, &input_selected);

  auto input_option = InputOption();
  std::string input_add_content;
  input_option.on_enter = [&] {
    input_entries.push_back(input_add_content);
    input_add_content = "";
  };
  Component input_add = Input(&input_add_content, "input files", input_option);

  std::string executable_content_ = "";
  Component executable_ = Input(&executable_content_, "executable");

  Component flags = Container::Vertical({
      Checkbox(&options_label[0], &options_state[0]),
      Checkbox(&options_label[1], &options_state[1]),
      Checkbox(&options_label[2], &options_state[2]),
      Checkbox(&options_label[3], &options_state[3]),
      Checkbox(&options_label[4], &options_state[4]),
      Checkbox(&options_label[5], &options_state[5]),
      Checkbox(&options_label[6], &options_state[6]),
      Checkbox(&options_label[7], &options_state[7]),
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

  auto render_command = [&] {
    Elements line;
    // Compiler
    line.push_back(text(compiler_entries[compiler_selected]) | bold);
    // flags
    for (int i = 0; i < 8; ++i) {
      if (options_state[i]) {
        line.push_back(text(" "));
        line.push_back(text(options_label[i]) | dim);
      }
    }
    // Executable
    if (!executable_content_.empty()) {
      line.push_back(text(" -o ") | bold);
      line.push_back(text(executable_content_) | color(Color::BlueLight) |
                     bold);
    }
    // Input
    for (auto& it : input_entries) {
      line.push_back(text(" " + it) | color(Color::RedLight));
    }
    return line;
  };

  auto compiler_renderer = Renderer(compiler_component, [&] {
    auto compiler_win = window(text("Compiler"),
                               compiler->Render() | vscroll_indicator | frame);
    auto flags_win =
        window(text("Flags"), flags->Render() | vscroll_indicator | frame);
    auto executable_win = window(text("Executable:"), executable_->Render());
    auto input_win =
        window(text("Input"), hbox({
                                  vbox({
                                      hbox({
                                          text("Add: "),
                                          input_add->Render(),
                                      }) | size(WIDTH, EQUAL, 20) |
                                          size(HEIGHT, EQUAL, 1),
                                      filler(),
                                  }),
                                  separator(),
                                  input->Render() | vscroll_indicator | frame |
                                      size(HEIGHT, EQUAL, 3) | flex,
                              }));
    return vbox({
               hbox({
                   compiler_win,
                   flags_win,
                   vbox({
                       executable_win | size(WIDTH, EQUAL, 20),
                       input_win | size(WIDTH, EQUAL, 60),
                   }),
                   filler(),
               }) | size(HEIGHT, LESS_THAN, 8),
               hflow(render_command()) | flex_grow,
           }) |
           flex_grow;
  });

  // ---------------------------------------------------------------------------
  // Spinner
  // ---------------------------------------------------------------------------
  auto spinner_tab_renderer = Renderer([&] {
    Elements entries;
    for (int i = 0; i < 22; ++i) {
      entries.push_back(spinner(i, shift / 2) | bold |
                        size(WIDTH, GREATER_THAN, 2) | border);
    }
    return hflow(std::move(entries));
  });

  // ---------------------------------------------------------------------------
  // Colors
  // ---------------------------------------------------------------------------
  auto color_tab_renderer = Renderer([] {
    auto basic_color_display =
        vbox({
            text("16 color palette:"),
            separator(),
            hbox({
                vbox({
                    color(Color::Default, text("Default")),
                    color(Color::Black, text("Black")),
                    color(Color::GrayDark, text("GrayDark")),
                    color(Color::GrayLight, text("GrayLight")),
                    color(Color::White, text("White")),
                    color(Color::Blue, text("Blue")),
                    color(Color::BlueLight, text("BlueLight")),
                    color(Color::Cyan, text("Cyan")),
                    color(Color::CyanLight, text("CyanLight")),
                    color(Color::Green, text("Green")),
                    color(Color::GreenLight, text("GreenLight")),
                    color(Color::Magenta, text("Magenta")),
                    color(Color::MagentaLight, text("MagentaLight")),
                    color(Color::Red, text("Red")),
                    color(Color::RedLight, text("RedLight")),
                    color(Color::Yellow, text("Yellow")),
                    color(Color::YellowLight, text("YellowLight")),
                }),
                vbox({
                    bgcolor(Color::Default, text("Default")),
                    bgcolor(Color::Black, text("Black")),
                    bgcolor(Color::GrayDark, text("GrayDark")),
                    bgcolor(Color::GrayLight, text("GrayLight")),
                    bgcolor(Color::White, text("White")),
                    bgcolor(Color::Blue, text("Blue")),
                    bgcolor(Color::BlueLight, text("BlueLight")),
                    bgcolor(Color::Cyan, text("Cyan")),
                    bgcolor(Color::CyanLight, text("CyanLight")),
                    bgcolor(Color::Green, text("Green")),
                    bgcolor(Color::GreenLight, text("GreenLight")),
                    bgcolor(Color::Magenta, text("Magenta")),
                    bgcolor(Color::MagentaLight, text("MagentaLight")),
                    bgcolor(Color::Red, text("Red")),
                    bgcolor(Color::RedLight, text("RedLight")),
                    bgcolor(Color::Yellow, text("Yellow")),
                    bgcolor(Color::YellowLight, text("YellowLight")),
                }),
            }),
        }) |
        border;

    auto palette_256_color_display = text("256 colors palette:");
    {
      std::vector<std::vector<ColorInfo>> info_columns = ColorInfoSorted2D();
      Elements columns;
      for (auto& column : info_columns) {
        Elements column_elements;
        for (auto& it : column) {
          column_elements.push_back(
              text("   ") | bgcolor(Color(Color::Palette256(it.index_256))));
        }
        columns.push_back(hbox(std::move(column_elements)));
      }
      palette_256_color_display = vbox({
                                      palette_256_color_display,
                                      separator(),
                                      vbox(columns),
                                  }) |
                                  border;
    }

    // True color display.
    auto true_color_display = text("TrueColors: 24bits:");
    {
      int saturation = 255;
      Elements array;
      for (int value = 0; value < 255; value += 16) {
        Elements line;
        for (int hue = 0; hue < 255; hue += 6) {
          line.push_back(text("▀")                                    //
                         | color(Color::HSV(hue, saturation, value))  //
                         | bgcolor(Color::HSV(hue, saturation, value + 8)));
        }
        array.push_back(hbox(std::move(line)));
      }
      true_color_display = vbox({
                               true_color_display,
                               separator(),
                               vbox(std::move(array)),
                           }) |
                           border;
    }

    return flexbox(
        {
            basic_color_display,
            palette_256_color_display,
            true_color_display,
        },
        FlexboxConfig().SetGap(1, 1));
  });

  // ---------------------------------------------------------------------------
  // Gauges
  // ---------------------------------------------------------------------------
  auto render_gauge = [&shift](int delta) {
    float progress = (shift + delta) % 500 / 500.f;
    return hbox({
        text(std::to_string(int(progress * 100)) + "% ") |
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
    });
  });

  // ---------------------------------------------------------------------------
  // Paragraph
  // ---------------------------------------------------------------------------
  auto make_box = [](size_t dimx, size_t dimy) {
    std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
    return window(text(title) | hcenter | bold,
                  text("content") | hcenter | dim) |
           size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
  };

  auto paragraph_renderer_left = Renderer([&] {
    std::string str =
        "Lorem Ipsum is simply dummy text of the printing and typesetting "
        "industry. Lorem Ipsum has been the industry's standard dummy text "
        "ever since the 1500s, when an unknown printer took a galley of type "
        "and scrambled it to make a type specimen book.";
    return vbox({
               window(text("Align left:"), paragraphAlignLeft(str)),
               window(text("Align center:"), paragraphAlignCenter(str)),
               window(text("Align right:"), paragraphAlignRight(str)),
               window(text("Align justify:"), paragraphAlignJustify(str)),
               window(text("Side by side"), hbox({
                                                paragraph(str),
                                                separator(),
                                                paragraph(str),
                                            })),
               window(text("Elements with different size:"),
                      flexbox({
                          make_box(10, 5),
                          make_box(9, 4),
                          make_box(8, 4),
                          make_box(6, 3),
                          make_box(10, 5),
                          make_box(9, 4),
                          make_box(8, 4),
                          make_box(6, 3),
                          make_box(10, 5),
                          make_box(9, 4),
                          make_box(8, 4),
                          make_box(6, 3),
                      })),
           }) |
           vscroll_indicator | yframe | flex;
  });

  auto paragraph_renderer_right = Renderer([] {
    return paragraph("<--- This vertical bar is resizable using the  mouse") |
           center;
  });

  int paragraph_renderer_split_position = Terminal::Size().dimx / 2;
  auto paragraph_renderer_group =
      ResizableSplitLeft(paragraph_renderer_left, paragraph_renderer_right,
                         &paragraph_renderer_split_position);
  auto paragraph_renderer_group_renderer =
      Renderer(paragraph_renderer_group,
               [&] { return paragraph_renderer_group->Render(); });

  // ---------------------------------------------------------------------------
  // Tabs
  // ---------------------------------------------------------------------------

  int tab_index = 0;
  std::vector<std::string> tab_entries = {
      "htop", "color", "spinner", "gauge", "compiler", "paragraph",
  };
  auto tab_selection =
      Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated());
  auto tab_content = Container::Tab(
      {
          htop,
          color_tab_renderer,
          spinner_tab_renderer,
          gauge_component,
          compiler_renderer,
          paragraph_renderer_group_renderer,
      },
      &tab_index);

  auto exit_button = Button(
      "Exit", [&] { screen.Exit(); }, ButtonOption::Animated());

  auto main_container = Container::Vertical({
      Container::Horizontal({
          tab_selection,
          exit_button,
      }),
      tab_content,
  });

  auto main_renderer = Renderer(main_container, [&] {
    return vbox({
        text("FTXUI Demo") | bold | hcenter,
        hbox({
            tab_selection->Render() | flex,
            exit_button->Render(),
        }),
        tab_content->Render() | flex,
    });
  });

  std::atomic<bool> refresh_ui_continue = true;
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      // The |shift| variable belong to the main thread. `screen.Post(task)`
      // will execute the update on the thread where |screen| lives (e.g. the
      // main thread). Using `screen.Post(task)` is threadsafe.
      screen.Post([&] { shift++; });
      // After updating the state, request a new frame to be drawn. This is done
      // by simulating a new "custom" event to be handled.
      screen.Post(Event::Custom);
    }
  });

  screen.Loop(main_renderer);
  refresh_ui_continue = false;
  refresh_ui.join();

  return 0;
}
