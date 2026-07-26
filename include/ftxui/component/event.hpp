// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_EVENT_HPP
#define FTXUI_COMPONENT_EVENT_HPP

#include <cstdint>
#include <ftxui/component/mouse.hpp>  // for Mouse
#include <memory>
#include <string>  // for string, operator==
#include <string_view>
#include <vector>

#include "ftxui/util/export.hpp"

namespace ftxui {

class App;
class ComponentBase;

/// @brief Represent an event. It can be key press event, a terminal resize, or
/// more ...
///
/// For example:
/// - Printable character can be created using Event::Character('a').
/// - Some special are predefined, like Event::ArrowLeft.
/// - One can find arbitrary code for special Events using:
///   ./example/util/print_key_press
///  For instance, CTLR+A maps to Event::Special({1});
///
/// Useful documentation about xterm specification:
/// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html
///
/// @ingroup component
struct FTXUI_EXPORT(COMPONENT) Event {
  // --- Constructor section ---------------------------------------------------
  static Event Character(std::string_view);
  static Event Character(char);
  static Event Character(wchar_t);
  static Event Special(std::string_view);
  static Event Special(std::initializer_list<char>);
  static Event Mouse(std::string_view, Mouse mouse);
  static Event CursorPosition(std::string_view, int x, int y);  // Internal
  static Event CursorShape(std::string_view, int shape);        // Internal
  static Event TerminalNameVersion(std::string_view,
                                   std::string name,
                                   int version);
  static Event TerminalEmulator(std::string_view,
                                std::string name,
                                std::string version);
  static Event TerminalCapabilities(std::string_view,
                                    std::vector<int> capabilities);

  // --- Arrow ---
  static const Event ArrowLeft;
  static const Event ArrowRight;
  static const Event ArrowUp;
  static const Event ArrowDown;

  static const Event ArrowLeftCtrl;
  static const Event ArrowRightCtrl;
  static const Event ArrowUpCtrl;
  static const Event ArrowDownCtrl;

  // --- Other ---
  static const Event Backspace;
  static const Event Delete;
  static const Event Return;
  static const Event Escape;
  static const Event Tab;
  static const Event TabReverse;

  // --- Navigation keys ---
  static const Event Insert;
  static const Event Home;
  static const Event End;
  static const Event PageUp;
  static const Event PageDown;

  // --- Function keys ---
  // MSVC's dllexport/dllimport only applies correctly to the first
  // declarator in a multi-name declaration, so these must be declared one
  // per statement (see error C2487) when built as a Windows DLL.
  static const Event F1;
  static const Event F2;
  static const Event F3;
  static const Event F4;
  static const Event F5;
  static const Event F6;
  static const Event F7;
  static const Event F8;
  static const Event F9;
  static const Event F10;
  static const Event F11;
  static const Event F12;

  // --- Control keys ---
  static const Event a;
  static const Event A;
  static const Event CtrlA;
  static const Event AltA;
  static const Event CtrlAltA;
  static const Event b;
  static const Event B;
  static const Event CtrlB;
  static const Event AltB;
  static const Event CtrlAltB;
  static const Event c;
  static const Event C;
  static const Event CtrlC;
  static const Event AltC;
  static const Event CtrlAltC;
  static const Event d;
  static const Event D;
  static const Event CtrlD;
  static const Event AltD;
  static const Event CtrlAltD;
  static const Event e;
  static const Event E;
  static const Event CtrlE;
  static const Event AltE;
  static const Event CtrlAltE;
  static const Event f;
  static const Event F;
  static const Event CtrlF;
  static const Event AltF;
  static const Event CtrlAltF;
  static const Event g;
  static const Event G;
  static const Event CtrlG;
  static const Event AltG;
  static const Event CtrlAltG;
  static const Event h;
  static const Event H;
  static const Event CtrlH;
  static const Event AltH;
  static const Event CtrlAltH;
  static const Event i;
  static const Event I;
  static const Event CtrlI;
  static const Event AltI;
  static const Event CtrlAltI;
  static const Event j;
  static const Event J;
  static const Event CtrlJ;
  static const Event AltJ;
  static const Event CtrlAltJ;
  static const Event k;
  static const Event K;
  static const Event CtrlK;
  static const Event AltK;
  static const Event CtrlAltK;
  static const Event l;
  static const Event L;
  static const Event CtrlL;
  static const Event AltL;
  static const Event CtrlAltL;
  static const Event m;
  static const Event M;
  static const Event CtrlM;
  static const Event AltM;
  static const Event CtrlAltM;
  static const Event n;
  static const Event N;
  static const Event CtrlN;
  static const Event AltN;
  static const Event CtrlAltN;
  static const Event o;
  static const Event O;
  static const Event CtrlO;
  static const Event AltO;
  static const Event CtrlAltO;
  static const Event p;
  static const Event P;
  static const Event CtrlP;
  static const Event AltP;
  static const Event CtrlAltP;
  static const Event q;
  static const Event Q;
  static const Event CtrlQ;
  static const Event AltQ;
  static const Event CtrlAltQ;
  static const Event r;
  static const Event R;
  static const Event CtrlR;
  static const Event AltR;
  static const Event CtrlAltR;
  static const Event s;
  static const Event S;
  static const Event CtrlS;
  static const Event AltS;
  static const Event CtrlAltS;
  static const Event t;
  static const Event T;
  static const Event CtrlT;
  static const Event AltT;
  static const Event CtrlAltT;
  static const Event u;
  static const Event U;
  static const Event CtrlU;
  static const Event AltU;
  static const Event CtrlAltU;
  static const Event v;
  static const Event V;
  static const Event CtrlV;
  static const Event AltV;
  static const Event CtrlAltV;
  static const Event w;
  static const Event W;
  static const Event CtrlW;
  static const Event AltW;
  static const Event CtrlAltW;
  static const Event x;
  static const Event X;
  static const Event CtrlX;
  static const Event AltX;
  static const Event CtrlAltX;
  static const Event y;
  static const Event Y;
  static const Event CtrlY;
  static const Event AltY;
  static const Event CtrlAltY;
  static const Event z;
  static const Event Z;
  static const Event CtrlZ;
  static const Event AltZ;
  static const Event CtrlAltZ;

  // --- Custom ---
  static const Event Custom;

  //--- Method section ---------------------------------------------------------
  bool operator==(const Event& other) const { return input_ == other.input_; }
  bool operator!=(const Event& other) const { return !operator==(other); }
  bool operator<(const Event& other) const { return input_ < other.input_; }

  const std::string& input() const { return input_; }

  bool is_character() const { return type_ == Type::Character; }
  std::string character() const { return input_; }

  bool is_mouse() const { return type_ == Type::Mouse; }
  struct Mouse& mouse() { return data_.mouse; }

  // --- Internal Method section -----------------------------------------------
  bool is_cursor_position() const { return type_ == Type::CursorPosition; }
  int cursor_x() const { return data_.cursor.x; }
  int cursor_y() const { return data_.cursor.y; }

  bool is_cursor_shape() const { return type_ == Type::CursorShape; }
  int cursor_shape() const { return data_.cursor_shape; }

  bool IsTerminalNameVersion() const;
  const std::string& TerminalName() const;
  int TerminalVersion() const;

  bool IsTerminalEmulator() const;
  const std::string& TerminalEmulatorName() const;
  const std::string& TerminalEmulatorVersion() const;

  bool IsTerminalCapabilities() const;
  const std::vector<int>& TerminalCapabilities() const;
  std::vector<std::string> TerminalCapabilityNames() const;

  // Debug
  std::string DebugString() const;

  //--- State section ----------------------------------------------------------
  App* screen_ = nullptr;

 private:
  friend ComponentBase;
  friend App;
  enum class Type : uint8_t {
    Unknown,
    Character,
    Mouse,
    CursorPosition,
    CursorShape,
    TerminalNameVersion,
    TerminalEmulator,
    TerminalCapabilities,
  };
  Type type_ = Type::Unknown;
  struct Cursor {
    int x = 0;
    int y = 0;
  };

  union {
    struct Mouse mouse;
    struct Cursor cursor;
    int cursor_shape;
    int terminal_version;
  } data_ = {};

  std::string input_;
  std::shared_ptr<std::string> terminal_name_;
  std::shared_ptr<std::string> terminal_emulator_version_;
  std::shared_ptr<std::vector<int>> terminal_capabilities_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_EVENT_HPP */
