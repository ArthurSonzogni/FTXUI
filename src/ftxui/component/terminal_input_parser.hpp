#ifndef FTXUI_COMPONENT_TERMINAL_INPUT_PARSER
#define FTXUI_COMPONENT_TERMINAL_INPUT_PARSER

#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/component/event.hpp"     // for Event (ptr only)
#include "ftxui/component/mouse.hpp"     // for Mouse
#include "ftxui/component/receiver.hpp"  // for Sender
#include "ftxui/component/task.hpp"      // for Task

namespace ftxui {
struct Event;

// Parse a sequence of |char| accross |time|. Produces |Event|.
class TerminalInputParser {
 public:
  TerminalInputParser(Sender<Task> out);
  void Timeout(int time);
  void Add(char c);

 private:
  unsigned char Current();
  bool Eat();

  enum Type {
    UNCOMPLETED,
    DROP,
    CHARACTER,
    SPECIAL,
    MOUSE,
    CURSOR_REPORTING,
  };

  struct CursorReporting {
    int x;
    int y;
  };

  struct Output {
    Type type;
    union {
      Mouse mouse;
      CursorReporting cursor;
    };

    Output(Type t) : type(t) {}
  };

  void Send(Output output);
  Output Parse();
  Output ParseUTF8();
  Output ParseESC();
  Output ParseDCS();
  Output ParseCSI();
  Output ParseOSC();
  Output ParseMouse(bool altered, bool pressed, std::vector<int> arguments);
  Output ParseCursorReporting(std::vector<int> arguments);

  Sender<Task> out_;
  int position_ = -1;
  int timeout_ = 0;
  std::string pending_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TERMINAL_INPUT_PARSER */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
