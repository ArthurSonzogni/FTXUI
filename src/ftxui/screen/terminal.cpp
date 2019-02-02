#include <iostream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "ftxui/screen/terminal.hpp"

namespace ftxui {

Terminal::Dimensions Terminal::Size() {
#ifdef __EMSCRIPTEN__
  return Dimensions{80,43};
#else
  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return Dimensions{w.ws_col, w.ws_row};
#endif
}

} // namespace ftxui
