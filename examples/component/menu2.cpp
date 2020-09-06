#include <chrono>
#include <iostream>
#include <thread>

#include "ftxui/component/container.hpp"
#include "ftxui/component/menu.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class MyComponent : public Component {
 public:
  MyComponent() {
    Add(&container);
    container.Add(&left_menu);
    container.Add(&right_menu);

    left_menu.entries = {
        L"0%",  L"10%", L"20%", L"30%", L"40%",
        L"50%", L"60%", L"70%", L"80%", L"90%",
    };
    right_menu.entries = {
        L"0%", L"1%", L"2%", L"3%", L"4%",  L"5%",
        L"6%", L"7%", L"8%", L"9%", L"10%",
    };

    left_menu.on_enter = [this]() { on_enter(); };
    right_menu.on_enter = [this]() { on_enter(); };
  }

  std::function<void()> on_enter = []() {};

 private:
  Container container = Container::Horizontal();
  Menu left_menu;
  Menu right_menu;

  Element Render() override {
    int sum = left_menu.selected * 10 + right_menu.selected;
    return border(vbox({
        // -------- Top panel --------------
        hbox({
            // -------- Left Menu --------------
            vbox({
                hcenter(bold(text(L"Percentage by 10%"))),
                separator(),
                left_menu.Render(),
            }) | flex,
            // -------- Right Menu --------------
            vbox({
                hcenter(bold(text(L"Percentage by 1%"))),
                separator(),
                right_menu.Render(),
            }) | flex,
            filler(),
        }),
        separator(),
        // -------- Bottom panel --------------
        vbox({
            hbox({
                text(L" gauge : "),
                gauge(sum / 100.0),
            }),
            hbox({
                text(L"  text : "),
                text(to_wstring(std::to_string(sum) + " %")),
            }),
        }) | flex,
    }));
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent component;
  component.on_enter = screen.ExitLoopClosure();
  screen.Loop(&component);
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
