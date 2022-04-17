#include <cstdint>  // for uint8_t
#include <iostream>  // for operator<<, stringstream, basic_ostream, flush, cout, ostream
#include <map>      // for _Rb_tree_const_iterator, map, operator!=, operator==
#include <memory>   // for allocator
#include <sstream>  // IWYU pragma: keep
#include <utility>  // for pair

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

Pixel& dev_null_pixel() {
  static Pixel pixel;
  return pixel;
}

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
  if (next == previous) {
    return;
  }

  if (next.bold && !previous.bold) {
    ss << "\x1B[1m";  // BOLD_SET
  }

  if (!next.bold && previous.bold) {
    ss << "\x1B[22m";  // BOLD_RESET
  }

  if (next.dim && !previous.dim) {
    ss << "\x1B[2m";  // DIM_SET
  }

  if (!next.dim && previous.dim) {
    ss << "\x1B[22m";  // DIM_RESET
  }

  if (next.underlined && !previous.underlined) {
    ss << "\x1B[4m";  // UNDERLINED_SET
  }

  if (!next.underlined && previous.underlined) {
    ss << "\x1B[24m";  // UNDERLINED_RESET
  }

  if (next.blink && !previous.blink) {
    ss << "\x1B[5m";  // BLINK_SET
  }

  if (!next.blink && previous.blink) {
    ss << "\x1B[25m";  // BLINK_RESET
  }

  if (next.inverted && !previous.inverted) {
    ss << "\x1B[7m";  // INVERTED_SET
  }

  if (!next.inverted && previous.inverted) {
    ss << "\x1B[27m";  // INVERTED_RESET
  }

  if (next.foreground_color != previous.foreground_color ||
      next.background_color != previous.background_color) {
    ss << "\x1B[" + next.foreground_color.Print(false) + "m";
    ss << "\x1B[" + next.background_color.Print(true) + "m";
  }

  previous = next;
}

struct TileEncoding {
  uint8_t left : 2;
  uint8_t top : 2;
  uint8_t right : 2;
  uint8_t down : 2;
  uint8_t round : 1;

  // clang-format off
  bool operator<(const TileEncoding& other) const {
    if (left < other.left)   { return  true;  }
    if (left > other.left)   { return  false; }
    if (top < other.top)     { return  true;  }
    if (top > other.top)     { return  false; }
    if (right < other.right) { return  true;  }
    if (right > other.right) { return  false; }
    if (down < other.down)   { return  true;  }
    if (down > other.down)   { return  false; }
    if (round < other.round) { return  true;  }
    if (round > other.round) { return  false; }
      return false;
  }
  // clang-format on
};

// clang-format off
const std::map<std::string, TileEncoding> tile_encoding = { // NOLINT
    {"─", {1, 0, 1, 0, 0}},
    {"━", {2, 0, 2, 0, 0}},

    {"│", {0, 1, 0, 1, 0}},
    {"┃", {0, 2, 0, 2, 0}},

    {"┌", {0, 0, 1, 1, 0}},
    {"┍", {0, 0, 2, 1, 0}},
    {"┎", {0, 0, 1, 2, 0}},
    {"┏", {0, 0, 2, 2, 0}},

    {"┐", {1, 0, 0, 1, 0}},
    {"┑", {2, 0, 0, 1, 0}},
    {"┒", {1, 0, 0, 2, 0}},
    {"┓", {2, 0, 0, 2, 0}},

    {"└", {0, 1, 1, 0, 0}},
    {"┕", {0, 1, 2, 0, 0}},
    {"┖", {0, 2, 1, 0, 0}},
    {"┗", {0, 2, 2, 0, 0}},

    {"┘", {1, 1, 0, 0, 0}},
    {"┙", {2, 1, 0, 0, 0}},
    {"┚", {1, 2, 0, 0, 0}},
    {"┛", {2, 2, 0, 0, 0}},

    {"├", {0, 1, 1, 1, 0}},
    {"┝", {0, 1, 2, 1, 0}},
    {"┞", {0, 2, 1, 1, 0}},
    {"┟", {0, 1, 1, 2, 0}},
    {"┠", {0, 2, 1, 2, 0}},
    {"┡", {0, 2, 2, 1, 0}},
    {"┢", {0, 1, 2, 2, 0}},
    {"┣", {0, 2, 2, 2, 0}},

    {"┤", {1, 1, 0, 1, 0}},
    {"┥", {2, 1, 0, 1, 0}},
    {"┦", {1, 2, 0, 1, 0}},
    {"┧", {1, 1, 0, 2, 0}},
    {"┨", {1, 2, 0, 2, 0}},
    {"┩", {2, 2, 0, 1, 0}},
    {"┪", {2, 1, 0, 2, 0}},
    {"┫", {2, 2, 0, 2, 0}},

    {"┬", {1, 0, 1, 1, 0}},
    {"┭", {2, 0, 1, 1, 0}},
    {"┮", {1, 0, 2, 1, 0}},
    {"┯", {2, 0, 2, 1, 0}},
    {"┰", {1, 0, 1, 2, 0}},
    {"┱", {2, 0, 1, 2, 0}},
    {"┲", {1, 0, 2, 2, 0}},
    {"┳", {2, 0, 2, 2, 0}},

    {"┴", {1, 1, 1, 0, 0}},
    {"┵", {2, 1, 1, 0, 0}},
    {"┶", {1, 1, 2, 0, 0}},
    {"┷", {2, 1, 2, 0, 0}},
    {"┸", {1, 2, 1, 0, 0}},
    {"┹", {2, 2, 1, 0, 0}},
    {"┺", {1, 2, 2, 0, 0}},
    {"┻", {2, 2, 2, 0, 0}},

    {"┼", {1, 1, 1, 1, 0}},
    {"┽", {2, 1, 1, 1, 0}},
    {"┾", {1, 1, 2, 1, 0}},
    {"┿", {2, 1, 2, 1, 0}},
    {"╀", {1, 2, 1, 1, 0}},
    {"╁", {1, 1, 1, 2, 0}},
    {"╂", {1, 2, 1, 2, 0}},
    {"╃", {2, 2, 1, 1, 0}},
    {"╄", {1, 2, 2, 1, 0}},
    {"╅", {2, 1, 1, 2, 0}},
    {"╆", {1, 1, 2, 2, 0}},
    {"╇", {2, 2, 2, 1, 0}},
    {"╈", {2, 1, 2, 2, 0}},
    {"╉", {2, 2, 1, 2, 0}},
    {"╊", {1, 2, 2, 2, 0}},
    {"╋", {2, 2, 2, 2, 0}},

    {"═", {3, 0, 3, 0, 0}},
    {"║", {0, 3, 0, 3, 0}},

    {"╒", {0, 0, 3, 1, 0}},
    {"╓", {0, 0, 1, 3, 0}},
    {"╔", {0, 0, 3, 3, 0}},

    {"╕", {3, 0, 0, 1, 0}},
    {"╖", {1, 0, 0, 3, 0}},
    {"╗", {3, 0, 0, 3, 0}},

    {"╘", {0, 1, 3, 0, 0}},
    {"╙", {0, 3, 1, 0, 0}},
    {"╚", {0, 3, 3, 0, 0}},

    {"╛", {3, 1, 0, 0, 0}},
    {"╜", {1, 3, 0, 0, 0}},
    {"╝", {3, 3, 0, 0, 0}},

    {"╞", {0, 1, 3, 1, 0}},
    {"╟", {0, 3, 1, 3, 0}},
    {"╠", {0, 3, 3, 3, 0}},

    {"╡", {3, 1, 0, 1, 0}},
    {"╢", {1, 3, 0, 3, 0}},
    {"╣", {3, 3, 0, 3, 0}},

    {"╤", {3, 0, 3, 1, 0}},
    {"╥", {1, 0, 1, 3, 0}},
    {"╦", {3, 0, 3, 3, 0}},

    {"╧", {3, 1, 3, 0, 0}},
    {"╨", {1, 3, 1, 0, 0}},
    {"╩", {3, 3, 3, 0, 0}},

    {"╪", {3, 1, 3, 1, 0}},
    {"╫", {1, 3, 1, 3, 0}},
    {"╬", {3, 3, 3, 3, 0}},

    {"╭", {0, 0, 1, 1, 1}},
    {"╮", {1, 0, 0, 1, 1}},
    {"╯", {1, 1, 0, 0, 1}},
    {"╰", {0, 1, 1, 0, 1}},

    {"╴", {1, 0, 0, 0, 0}},
    {"╵", {0, 1, 0, 0, 0}},
    {"╶", {0, 0, 1, 0, 0}},
    {"╷", {0, 0, 0, 1, 0}},

    {"╸", {2, 0, 0, 0, 0}},
    {"╹", {0, 2, 0, 0, 0}},
    {"╺", {0, 0, 2, 0, 0}},
    {"╻", {0, 0, 0, 2, 0}},

    {"╼", {1, 0, 2, 0, 0}},
    {"╽", {0, 1, 0, 2, 0}},
    {"╾", {2, 0, 1, 0, 0}},
    {"╿", {0, 2, 0, 1, 0}},
};
// clang-format on

template <class A, class B>
std::map<B, A> InvertMap(const std::map<A, B> input) {
  std::map<B, A> output;
  for (const auto& it : input) {
    output[it.second] = it.first;
  }
  return output;
}

const std::map<TileEncoding, std::string> tile_encoding_inverse =  // NOLINT
    InvertMap(tile_encoding);

void UpgradeLeftRight(std::string& left, std::string& right) {
  const auto it_left = tile_encoding.find(left);
  if (it_left == tile_encoding.end()) {
    return;
  }
  const auto it_right = tile_encoding.find(right);
  if (it_right == tile_encoding.end()) {
    return;
  }

  if (it_left->second.right == 0 && it_right->second.left != 0) {
    TileEncoding encoding_left = it_left->second;
    encoding_left.right = it_right->second.left;
    const auto it_left_upgrade = tile_encoding_inverse.find(encoding_left);
    if (it_left_upgrade != tile_encoding_inverse.end()) {
      left = it_left_upgrade->second;
    }
  }

  if (it_right->second.left == 0 && it_left->second.right != 0) {
    TileEncoding encoding_right = it_right->second;
    encoding_right.left = it_left->second.right;
    const auto it_right_upgrade = tile_encoding_inverse.find(encoding_right);
    if (it_right_upgrade != tile_encoding_inverse.end()) {
      right = it_right_upgrade->second;
    }
  }
}

void UpgradeTopDown(std::string& top, std::string& down) {
  const auto it_top = tile_encoding.find(top);
  if (it_top == tile_encoding.end()) {
    return;
  }
  const auto it_down = tile_encoding.find(down);
  if (it_down == tile_encoding.end()) {
    return;
  }

  if (it_top->second.down == 0 && it_down->second.top != 0) {
    TileEncoding encoding_top = it_top->second;
    encoding_top.down = it_down->second.top;
    const auto it_top_down = tile_encoding_inverse.find(encoding_top);
    if (it_top_down != tile_encoding_inverse.end()) {
      top = it_top_down->second;
    }
  }

  if (it_down->second.top == 0 && it_top->second.down != 0) {
    TileEncoding encoding_down = it_down->second;
    encoding_down.top = it_top->second.down;
    const auto it_down_top = tile_encoding_inverse.find(encoding_down);
    if (it_down_top != tile_encoding_inverse.end()) {
      down = it_down_top->second;
    }
  }
}

bool ShouldAttemptAutoMerge(Pixel& pixel) {
  return pixel.automerge && pixel.character.size() == 3;
}

}  // namespace

bool Pixel::operator==(const Pixel& other) const {
  return character == other.character &&                //
         background_color == other.background_color &&  //
         foreground_color == other.foreground_color &&  //
         blink == other.blink &&                        //
         bold == other.bold &&                          //
         dim == other.dim &&                            //
         inverted == other.inverted &&                  //
         underlined == other.underlined &&              //
         automerge == other.automerge;                  //
}

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
  return stencil.Contain(x, y) ? pixels_[y][x] : dev_null_pixel();
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
std::string Screen::ResetPosition(bool clear) const {
  std::stringstream ss;
  if (clear) {
    ss << "\r";       // MOVE_LEFT;
    ss << "\x1b[2K";  // CLEAR_SCREEN;
    for (int y = 1; y < dimy_; ++y) {
      ss << "\x1B[1A";  // MOVE_UP;
      ss << "\x1B[2K";  // CLEAR_LINE;
    }
  } else {
    ss << "\r";  // MOVE_LEFT;
    for (int y = 1; y < dimy_; ++y) {
      ss << "\x1B[1A";  // MOVE_UP;
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
      Pixel& cur = pixels_[y][x];
      if (!ShouldAttemptAutoMerge(cur)) {
        continue;
      }

      Pixel& left = pixels_[y][x-1];
      Pixel& top = pixels_[y-1][x];

      if (ShouldAttemptAutoMerge(left)) {
        UpgradeLeftRight(left.character, cur.character);
      }
      if (ShouldAttemptAutoMerge(top)) {
        UpgradeTopDown(top.character, cur.character);
      }
    }
  }
}

// clang-format on

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
