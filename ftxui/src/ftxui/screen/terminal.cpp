#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "ftxui/screen/terminal.hpp"

namespace ftxui {

Terminal::Dimensions Terminal::Size() {
  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return Dimensions{w.ws_col, w.ws_row};
}

} // namespace ftxui
