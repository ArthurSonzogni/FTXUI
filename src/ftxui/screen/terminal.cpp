#include <cstdlib>  // for getenv
#include <string>   // for string, allocator

#include "ftxui/screen/terminal.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#else
#include <sys/ioctl.h>  // for winsize, ioctl, TIOCGWINSZ
#include <unistd.h>     // for STDOUT_FILENO
#endif

namespace ftxui {

Dimensions Terminal::Size() {
#if defined(__EMSCRIPTEN__)
  return Dimensions{140, 43};
#elif defined(_WIN32)
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return Dimensions{csbi.srWindow.Right - csbi.srWindow.Left + 1,
                      csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
  }

  // The Microsoft default "cmd" returns errors above.
  return Dimensions{80, 80};

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
#if defined(__EMSCRIPTEN__)
  return Terminal::Color::TrueColor;
#endif

  std::string COLORTERM = Safe(std::getenv("COLORTERM"));
  if (Contains(COLORTERM, "24bit") || Contains(COLORTERM, "truecolor"))
    return Terminal::Color::TrueColor;

  std::string TERM = Safe(std::getenv("TERM"));
  if (Contains(COLORTERM, "256") || Contains(TERM, "256"))
    return Terminal::Color::Palette256;

#if defined(FTXUI_MICROSOFT_TERMINAL_FALLBACK)
  // Microsoft terminals do not properly declare themselve supporting true
  // colors: https://github.com/microsoft/terminal/issues/1040
  // As a fallback, assume microsoft terminal are the ones not setting those
  // variables, and enable true colors.
  if (TERM == "" && COLORTERM == "")
    return Terminal::Color::TrueColor;
#endif

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
