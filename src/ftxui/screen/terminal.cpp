#include "ftxui/screen/terminal.hpp"

#include <stdio.h>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <Windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

#include <iostream>

namespace ftxui {

Terminal::Dimensions Terminal::Size() {
#if defined(__EMSCRIPTEN__)
  return Dimensions{80, 43};
#elif defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns, rows;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  return Dimensions{columns, rows};
#else
  winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return Dimensions{w.ws_col, w.ws_row};
#endif
}

}  // namespace ftxui
