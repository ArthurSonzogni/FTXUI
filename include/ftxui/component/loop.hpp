// Copyright 2022 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_LOOP_HPP
#define FTXUI_COMPONENT_LOOP_HPP

#include <memory>  // for shared_ptr

#include "ftxui/component/component_base.hpp"  // for ComponentBase

namespace ftxui {
class ComponentBase;

using Component = std::shared_ptr<ComponentBase>;
class ScreenInteractive;

class Loop {
 public:
  Loop(ScreenInteractive* screen, Component component);
  ~Loop();

  bool HasQuitted();
  void RunOnce();
  void RunOnceBlocking();
  void Run();

 private:
  // This class is non copyable.
  Loop(const ScreenInteractive&) = delete;
  Loop& operator=(const Loop&) = delete;

  ScreenInteractive* screen_;
  Component component_;
};

}  // namespace ftxui

#endif  // FTXUI_COMPONENT_LOOP_HPP
