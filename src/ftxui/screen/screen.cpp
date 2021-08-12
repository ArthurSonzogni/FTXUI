#include <iostream>  // for operator<<, stringstream, basic_ostream, flush, cout, ostream
#include <memory>   // for allocator
#include <sstream>  // IWYU pragma: keep

#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"    // for string_width
#include "ftxui/screen/terminal.hpp"  // for Dimensions, Size

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace ftxui {

namespace {
static const char BOLD_SET[] = "\x1B[1m";
static const char BOLD_RESET[] = "\x1B[22m";  // Can't use 21 here.

static const char DIM_SET[] = "\x1B[2m";
static const char DIM_RESET[] = "\x1B[22m";

static const char UNDERLINED_SET[] = "\x1B[4m";
static const char UNDERLINED_RESET[] = "\x1B[24m";

static const char BLINK_SET[] = "\x1B[5m";
static const char BLINK_RESET[] = "\x1B[25m";

static const char INVERTED_SET[] = "\x1B[7m";
static const char INVERTED_RESET[] = "\x1B[27m";

static const char MOVE_LEFT[] = "\r";
static const char MOVE_UP[] = "\x1B[1A";
static const char CLEAR_LINE[] = "\x1B[2K";

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

void UpdatePixelStyle(std::stringstream& ss,
                      Pixel& previous,
                      const Pixel& next) {
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
    ss << "\x1B[" + next.foreground_color.Print(false) + "m";
    ss << "\x1B[" + next.background_color.Print(true) + "m";
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
  std::stringstream ss;

  Pixel previous_pixel;
  Pixel final_pixel;

  for (int y = 0; y < dimy_; ++y) {
    if (y != 0) {
      UpdatePixelStyle(ss, previous_pixel, final_pixel);
      ss << "\r\n";
    }
    bool previous_fullwidth = false;
    for (const auto& pixel : pixels_[y]) {
      if (!previous_fullwidth) {
        UpdatePixelStyle(ss, previous_pixel, pixel);
        ss << pixel.character;
      }
      previous_fullwidth = (string_width(pixel.character) == 2);
    }
  }

  UpdatePixelStyle(ss, previous_pixel, final_pixel);

  return ss.str();
}

void Screen::Print() {
  std::cout << ToString() << '\0' << std::flush;
}

/// @brief Access a character a given position.
/// @param x The character position along the x-axis.
/// @param y The character position along the y-axis.
std::string& Screen::at(int x, int y) {
  return PixelAt(x, y).character;
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
      // Box drawing character uses exactly 3 byte.
      std::string& cur = pixels_[y][x].character;
      if (cur.size() != 3u)
        continue;

      // Left vs current.
      std::string& left = pixels_[y][x-1].character;
      if (left.size() == 3u) {
        if (cur == "│" && left == "─") cur = "┤";
        if (cur == "├" && left == "─") cur = "┼";
        if (cur == "─" && left == "│") left = "├";
        if (cur == "─" && left == "┤") left = "┼";
      }

      // Top vs current.
      std::string& top = pixels_[y-1][x].character;
      if (top.size() == 3u) {
        if (cur == "─" && top == "│") cur = "┴";
        if (cur == "┬" && top == "│") cur = "┼";
        if (cur == "│" && top == "─") top = "┬";
        if (cur == "│" && top == "┴") top = "┼";
      }
    }
  }
}

// clang-format on

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
