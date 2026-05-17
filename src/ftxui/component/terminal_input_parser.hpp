// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_TERMINAL_INPUT_PARSER
#define FTXUI_COMPONENT_TERMINAL_INPUT_PARSER

#include <functional>
#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/component/mouse.hpp"  // for Mouse

namespace ftxui {
struct Event;

// Parse a sequence of |char| across |time|. Produces |Event|.
class TerminalInputParser {
 public:
  explicit TerminalInputParser(std::function<void(Event)> out);
  void Timeout(int time);
  void Add(char c);

 private:
  unsigned char Current();
  bool Eat();

  enum Type {
    UNCOMPLETED,
    DROP,
    CHARACTER,
    MOUSE,
    CURSOR_POSITION,
    CURSOR_SHAPE,
    TERMINAL_NAME_VERSION,
    TERMINAL_EMULATOR,
    TERMINAL_CAPABILITIES,
    SPECIAL,
  };

  struct CursorPosition {
    int x;
    int y;
  };

  struct TerminalNameVersion {
    std::string name;
    int version;
  };

  struct TerminalEmulator {
    std::string name;
    std::string version;
  };

  struct TerminalCapabilities {
    std::vector<int> capabilities;
  };

  struct Output {
    Type type;
    union {
      Mouse mouse;
      CursorPosition cursor{};
      int cursor_shape;
      int terminal_version;
    };
    std::string terminal_name;
    std::string terminal_version_string;
    std::vector<int> terminal_capabilities;

    Output(Type t)  // NOLINT
        : type(t) {}
  };

  void Send(Output output);
  Output Parse();
  Output ParseUTF8();
  Output ParseESC();
  Output ParseDCS();
  Output ParseCSI();
  Output ParseOSC();
  Output ParseMouse(bool altered, bool pressed, std::vector<int> arguments);
  Output ParseCursorPosition(std::vector<int> arguments);
  Output ParseDeviceAttributes(bool altered_greater,
                               bool altered_question,
                               std::vector<int> arguments);

  std::function<void(Event)> out_;
  int position_ = -1;
  int timeout_ = 0;
  std::string pending_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_TERMINAL_INPUT_PARSER */
