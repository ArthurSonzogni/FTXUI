#include "ftxui/screen/terminal.hpp"

#include <stdio.h>

#include <cstdlib>

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

namespace {

const char* Safe(const char* c) {
  return c ? c : "";
}

bool Contains(const std::string& s, const char* key) {
  return s.find(key) != std::string::npos;
}

static bool cached = false;
Terminal::Color cached_supported_color;
Terminal::Color ComputeColorSupport() {
  std::string COLORTERM = Safe(std::getenv("COLORTERM"));
  if (Contains(COLORTERM, "24bit") || Contains(COLORTERM, "truecolor"))
    return Terminal::Color::TrueColor;

  std::string TERM = Safe(std::getenv("TERM"));
  if (Contains(COLORTERM, "256") || Contains(TERM, "256"))
    return Terminal::Color::Palette256;

  return Terminal::Color::Palette16;
}

}  // namespace

Terminal::Color Terminal::ColorSupport() {
  if (!cached) {
    cached = true;
    cached_supported_color = ComputeColorSupport();
  }
  return cached_supported_color;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
