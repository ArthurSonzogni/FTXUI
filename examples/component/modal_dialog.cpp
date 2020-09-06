#include "ftxui/component/button.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

// The main screen, at depth 0. It display the main content.
class Content : public Component {
 public:
  std::function<void()> on_rate_ftxui = [] {};
  std::function<void()> on_quit = [] {};
  std::wstring rating_ = L"3/5 stars";
  Content() {
    Add(&container_);
    container_.Add(&button_rate_ftxui);
    container_.Add(&button_quit_);
    button_rate_ftxui.on_click = [&] { on_rate_ftxui(); };
    button_quit_.on_click = [&] { on_quit(); };
  }

  Element Render() final {
    auto button_elements = hbox({
        button_rate_ftxui.Render(),
        filler(),
        button_quit_.Render(),
    });

    auto document =  //
        vbox({
            text(L"Modal dialog example"),
            separator(),
            text(L"☆☆☆ FTXUI:" + rating_ + L" ☆☆☆") | bold,
            filler(),
            button_elements,
        }) |
        border;

    return document | size(HEIGHT, GREATER_THAN, 18) | center;
  }

 private:
  Container container_ = Container::Horizontal();
  Button button_rate_ftxui = Button(L"Rate FTXUI");
  Button button_quit_ = Button(L"Quit");
};

// The "modal" screen, at depth 1. It display the modal dialog.
class Modal : public Component {
 public:
  std::function<void(std::wstring)> on_click;

  Modal() {
    Add(&container_);
    buttons_.resize(5);
    for (int i = 0; i < 5; ++i) {
      std::wstring stars = std::to_wstring(i + 1) + L"/5 stars";
      buttons_[i] = Button(stars);
      buttons_[i].on_click = [&, stars] { on_click(stars); };
      container_.Add(&buttons_[i]);
    }
  }

  Element Render() final {
    return vbox({
               text(L"Do you like FTXUI?"),
               separator(),
               hbox({
                   buttons_[0].Render(),
                   buttons_[1].Render(),
                   buttons_[2].Render(),
                   buttons_[3].Render(),
                   buttons_[4].Render(),
               }),
           }) |
           border;
  }

 private:
  Container container_ = Container::Horizontal();
  std::vector<Button> buttons_;
};

class MyComponent : public Component {
 public:
  std::function<void()> on_quit = [] {};

  MyComponent() {
    Add(&container_);
    container_.Add(&content_);
    container_.Add(&modal_);

    content_.on_quit = [&] { on_quit(); };
    content_.on_rate_ftxui = [&] { modal_.TakeFocus(); };
    modal_.on_click = [&](std::wstring rating) {
      content_.rating_ = rating;
      content_.TakeFocus();
    };
  }

  Element Render() final {
    Element document = content_.Render();
    if (modal_.Focused()) {
      document = dbox({
          document,
          modal_.Render() | clear_under | center,
      });
    }
    return document;
  }

 private:
  Container container_ = Container::Tab(nullptr);
  Content content_;
  Modal modal_;
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  MyComponent my_component;
  my_component.on_quit = screen.ExitLoopClosure();
  screen.Loop(&my_component);
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
