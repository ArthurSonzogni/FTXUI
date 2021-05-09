#include <functional>  // for function
#include <memory>  // for allocator, __shared_ptr_access, shared_ptr, make_shared
#include <string>  // for wstring, operator+, basic_string, char_traits
#include <vector>  // for vector

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Button, Make
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/container.hpp"       // for Container
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for Element, operator|, filler, text, hbox, separator, center, vbox, bold, border, clear_under, dbox, size, GREATER_THAN, HEIGHT

using namespace ftxui;

// The main screen, at depth 0. It display the main content.
class Content : public ComponentBase {
 private:
  std::wstring label_rate_ftxui_ = L"Rate FTXUI";
  std::wstring label_quit_ = L"Quit";
  bool modal_open_ = false;

  Component button_rate_ftxui_ =
      Button(&label_rate_ftxui_, [this] { on_rate_ftxui(); });
  Component button_quit_ = Button(&label_quit_, [this] { on_quit(); });
  Component container_ = Container::Horizontal({
      button_rate_ftxui_,
      button_quit_,
  });

 public:
  std::wstring rating = L"3/5 stars";
  std::function<void()> on_rate_ftxui;
  std::function<void()> on_quit;

  Content() { Add(container_); }

  Element Render() final {
    auto document =  //
        vbox({
            text(L"Modal dialog example"),
            separator(),
            text(L"☆☆☆ FTXUI:" + rating + L" ☆☆☆") | bold,
            filler(),
            hbox({
                button_rate_ftxui_->Render(),
                filler(),
                button_quit_->Render(),
            }),
        }) |
        border;

    return document | size(HEIGHT, GREATER_THAN, 18) | center;
  }
};

std::vector<std::wstring> rating_labels = {
    L"1/5 stars", L"2/5 stars", L"3/5 stars", L"4/5 stars", L"5/5 stars",
};

// The "modal" screen, at depth 1. It display the modal dialog.
class Modal : public ComponentBase {
 private:
  Component container_ = Container::Horizontal({
      Button(&rating_labels[0], [this] { on_click(rating_labels[0]); }),
      Button(&rating_labels[1], [this] { on_click(rating_labels[1]); }),
      Button(&rating_labels[2], [this] { on_click(rating_labels[2]); }),
      Button(&rating_labels[3], [this] { on_click(rating_labels[3]); }),
      Button(&rating_labels[4], [this] { on_click(rating_labels[4]); }),
  });

 public:
  std::function<void(std::wstring)> on_click;

  Modal() { Add(container_); }

  Element Render() final {
    return vbox({
               text(L"Do you like FTXUI?"),
               separator(),
               hbox(container_->Render()),
           }) |
           border;
  }
};

class MyComponent : public ComponentBase {
 private:
  std::shared_ptr<Content> content_ = std::make_shared<Content>();
  std::shared_ptr<Modal> modal_ = std::make_shared<Modal>();

  int depth = 0;
  Component container_ = Container::Tab(&depth,
                                        {
                                            content_,
                                            modal_,
                                        });

  std::function<void()> on_quit_;

 public:
  MyComponent(std::function<void()> on_quit) : on_quit_(on_quit) {
    Add(container_);

    content_->on_quit = [&] { on_quit(); };
    content_->on_rate_ftxui = [this] { depth = 1; };
    modal_->on_click = [&](std::wstring rating) {
      content_->rating = rating;
      depth = 0;
    };
  }

  Element Render() final {
    Element document = content_->Render();

    if (depth == 1) {
      document = dbox({
          document,
          modal_->Render() | clear_under | center,
      });
    }
    return document;
  }
};

int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(Make<MyComponent>(screen.ExitLoopClosure()));
  return 0;
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
