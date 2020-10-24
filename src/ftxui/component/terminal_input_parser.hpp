#ifndef FTXUI_COMPONENT_TERMINAL_INPUT_PARSER
#define FTXUI_COMPONENT_TERMINAL_INPUT_PARSER

#include "ftxui/component/event.hpp"
#include "ftxui/component/receiver.hpp"

#include <string>

namespace ftxui {

// Parse a sequence of |char| accross |time|. Produces |Event|.
class TerminalInputParser {
 public:
  TerminalInputParser(Sender<Event> out);
  void Timeout(int time);
  void Add(char c);

 private:
  unsigned char Current();
  bool Eat();

  enum Type {
    UNCOMPLETED = 0,
    DROP = 1,
    CHARACTER = 2,
    SPECIAL = 3,
  };
  void Send(Type type);
  Type Parse();
  Type ParseUTF8();
  Type ParseESC();
  Type ParseDCS();
  Type ParseCSI();
  Type ParseOSC();

  Sender<Event> out_;
  int position_ = -1;
  int timeout_ = 0;
  std::string pending_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TERMINAL_INPUT_PARSER */

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
