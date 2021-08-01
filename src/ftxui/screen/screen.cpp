#include <algorithm>  // for min
#include <iostream>  // for operator<<, basic_ostream, wstringstream, stringstream, flush, cout, ostream
#include <sstream>   // IWYU pragma: keep

#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"    // for to_string, wchar_width
#include "ftxui/screen/terminal.hpp"  // for Terminal::Dimensions, Terminal

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace ftxui {

namespace {
static const wchar_t* BOLD_SET = L"\x1B[1m";
static const wchar_t* BOLD_RESET = L"\x1B[22m";  // Can't use 21 here.

static const wchar_t* DIM_SET = L"\x1B[2m";
static const wchar_t* DIM_RESET = L"\x1B[22m";

static const wchar_t* UNDERLINED_SET = L"\x1B[4m";
static const wchar_t* UNDERLINED_RESET = L"\x1B[24m";

static const wchar_t* BLINK_SET = L"\x1B[5m";
static const wchar_t* BLINK_RESET = L"\x1B[25m";

static const wchar_t* INVERTED_SET = L"\x1B[7m";
static const wchar_t* INVERTED_RESET = L"\x1B[27m";

static const char* MOVE_LEFT = "\r";
static const char* MOVE_UP = "\x1B[1A";
static const char* CLEAR_LINE = "\x1B[2K";

Pixel dev_null_pixel;

#if defined(_WIN32)
void WindowsEmulateVT100Terminal() {
  static bool done = false;
  if (done)
    return;
  done = true;

  // Enable VT processing on stdout and stdin
  auto stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD out_mode = 0;
  GetConsoleMode(stdout_handle, &out_mode);

  // https://docs.microsoft.com/en-us/windows/console/setconsolemode
  const int enable_virtual_terminal_processing = 0x0004;
  const int disable_newline_auto_return = 0x0008;
  out_mode |= enable_virtual_terminal_processing;
  out_mode |= disable_newline_auto_return;

  SetConsoleMode(stdout_handle, out_mode);
}
#endif

void UpdatePixelStyle(std::wstringstream& ss, Pixel& previous, Pixel& next) {
  if (next.bold != previous.bold)
    ss << (next.bold ? BOLD_SET : BOLD_RESET);

  if (next.dim != previous.dim)
    ss << (next.dim ? DIM_SET : DIM_RESET);

  if (next.underlined != previous.underlined)
    ss << (next.underlined ? UNDERLINED_SET : UNDERLINED_RESET);

  if (next.blink != previous.blink)
    ss << (next.blink ? BLINK_SET : BLINK_RESET);

  if (next.inverted != previous.inverted)
    ss << (next.inverted ? INVERTED_SET : INVERTED_RESET);

  if (next.foreground_color != previous.foreground_color ||
      next.background_color != previous.background_color) {
    ss << L"\x1B[" + next.foreground_color.Print(false) + L"m";
    ss << L"\x1B[" + next.background_color.Print(true) + L"m";
  }

  previous = next;
}

}  // namespace

/// A fixed dimension.
/// @see Fit
/// @see Full
Dimensions Dimension::Fixed(int v) {
  return {v, v};
}

/// Use the terminal dimensions.
/// @see Fixed
/// @see Fit
Dimensions Dimension::Full() {
  return Terminal::Size();
}

// static
/// Create a screen with the given dimension along the x-axis and y-axis.
Screen Screen::Create(Dimensions width, Dimensions height) {
  return Screen(width.dimx, height.dimy);
}

// static
/// Create a screen with the given dimension.
Screen Screen::Create(Dimensions dimension) {
  return Screen(dimension.dimx, dimension.dimy);
}

Screen::Screen(int dimx, int dimy)
    : stencil{0, dimx - 1, 0, dimy - 1},
      dimx_(dimx),
      dimy_(dimy),
      pixels_(dimy, std::vector<Pixel>(dimx)) {
#if defined(_WIN32)
  // The placement of this call is a bit weird, however we can assume that
  // anybody who instantiates a Screen object eventually wants to output
  // something to the console.
  // As we require UTF8 for all input/output operations we will just switch to
  // UTF8 encoding here
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  WindowsEmulateVT100Terminal();
#endif
}

/// Produce a std::string that can be used to print the Screen on the terminal.
/// Don't forget to flush stdout. Alternatively, you can use Screen::Print();
std::string Screen::ToString() {
  std::wstringstream ss;

  Pixel previous_pixel;
  Pixel final_pixel;

  for (int y = 0; y < dimy_; ++y) {
    if (y != 0) {
      UpdatePixelStyle(ss, previous_pixel, final_pixel);
      ss << L"\r\n";
    }
    for (int x = 0; x < dimx_;) {
      auto& pixel = pixels_[y][x];
      UpdatePixelStyle(ss, previous_pixel, pixel);

      int x_inc = 0;
      for (auto& c : pixel.character) {
        ss << c;
        x_inc += wchar_width(c);
      }
      x += std::max(x_inc, 1);
    }
  }

  UpdatePixelStyle(ss, previous_pixel, final_pixel);

  return to_string(ss.str());
}

void Screen::Print() {
  std::cout << ToString() << '\0' << std::flush;
}

/// @brief Access a character a given position.
/// @param x The character position along the x-axis.
/// @param y The character position along the y-axis.
wchar_t& Screen::at(int x, int y) {
  return PixelAt(x, y).character[0];
}

/// @brief Access a Pixel at a given position.
/// @param x The pixel position along the x-axis.
/// @param y The pixel position along the y-axis.
Pixel& Screen::PixelAt(int x, int y) {
  return stencil.Contain(x, y) ? pixels_[y][x] : dev_null_pixel;
}

/// @brief Return a string to be printed in order to reset the cursor position
///        to the beginning of the screen.
///
/// ```cpp
/// std::string reset_position;
/// while(true) {
///   auto document = render();
///   auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
///   Render(screen, document);
///   std::cout << reset_position << screen.ToString() << std::flush;
///   reset_position = screen.ResetPosition();
///
///   using namespace std::chrono_literals;
///   std::this_thread::sleep_for(0.01s);
/// }
/// ```
///
/// @return The string to print in order to reset the cursor position to the
///         beginning.
std::string Screen::ResetPosition(bool clear) {
  std::stringstream ss;
  if (clear) {
    ss << MOVE_LEFT << CLEAR_LINE;
    for (int y = 1; y < dimy_; ++y) {
      ss << MOVE_UP << CLEAR_LINE;
    }
  } else {
    ss << MOVE_LEFT;
    for (int y = 1; y < dimy_; ++y) {
      ss << MOVE_UP;
    }
  }
  return ss.str();
}

/// @brief Clear all the pixel from the screen.
void Screen::Clear() {
  pixels_ = std::vector<std::vector<Pixel>>(dimy_,
                                            std::vector<Pixel>(dimx_, Pixel()));
  cursor_.x = dimx_ - 1;
  cursor_.y = dimy_ - 1;
}

// clang-format off
void Screen::ApplyShader() {
  // Merge box characters togethers.
  for (int y = 1; y < dimy_; ++y) {
    for (int x = 1; x < dimx_; ++x) {
      wchar_t& left = at(x - 1, y);
      wchar_t& top = at(x, y - 1);
      wchar_t& cur = at(x, y);

      // Left vs current
      if (cur == U'│' && left == U'─') cur = U'┤';
      if (cur == U'─' && left == U'│') left = U'├';
      if (cur == U'├' && left == U'─') cur = U'┼';
      if (cur == U'─' && left == U'┤') left = U'┼';

      // Top vs current
      if (cur == U'─' && top == U'│') cur = U'┴';
      if (cur == U'│' && top == U'─') top = U'┬';
      if (cur == U'┬' && top == U'│') cur = U'┼';
      if (cur == U'│' && top == U'┴') top = U'┼';
    }
  }
}

// clang-format on

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
