#include "ftxui/dom/node.hpp"
#include "ftxui/screen.hpp"
#include "ftxui/terminal.hpp"
#include "ftxui/util/string.hpp"

#include <sstream>

namespace ftxui {

Screen::Screen(size_t dimx, size_t dimy)
    : dimx_(dimx), dimy_(dimy), pixels_(dimy, std::vector<Pixel>(dimx)) {}

std::string Screen::ToString() {
  std::wstringstream ss;

  Pixel previous_pixel;

  for (size_t y = 0; y < dimy_; ++y) {
    for (size_t x = 0; x < dimx_; ++x) {
      if (pixels_[y][x].bold != previous_pixel.bold) {
        if (pixels_[y][x].bold) {
          ss << L"\e[1m";
        } else {
          ss << L"\e[0m";
        }
      }
      if (pixels_[y][x].inverted != previous_pixel.inverted) {
        if (pixels_[y][x].inverted) {
          ss << L"\e[7m";
        } else {
          ss << L"\e[27m";
        }
      }
      if (pixels_[y][x].underlined != previous_pixel.underlined) {
        if (pixels_[y][x].underlined) {
          ss << L"\e[4m";
        } else {
          ss << L"\e[24m";
        }
      }
      if (pixels_[y][x].dim != previous_pixel.dim) {
        if (pixels_[y][x].dim) {
          ss << L"\e[2m";
        } else {
          ss << L"\e[22m";
        }
      }
      if (pixels_[y][x].blink != previous_pixel.blink) {
        if (pixels_[y][x].blink) {
          ss << L"\e[5m";
        } else {
          ss << L"\e[25m";
        }
      }
      if (pixels_[y][x].foreground_color != previous_pixel.foreground_color) {
        ss << L"\e[" + to_wstring(std::to_string(
                           (uint8_t)pixels_[y][x].foreground_color)) +
                  L"m";
      }
      if (pixels_[y][x].background_color != previous_pixel.background_color) {
        ss << L"\e[" + to_wstring(std::to_string(
                           10 + (uint8_t)pixels_[y][x].background_color)) +
                  L"m";
      }
      ss << pixels_[y][x].character;
      previous_pixel = pixels_[y][x];
    }
    if (y + 1 < dimy_)
      ss << '\n';
  }
  return to_string(ss.str());
}

wchar_t& Screen::at(size_t x, size_t y) {
  return pixels_[y][x].character;
}

Pixel& Screen::PixelAt(size_t x, size_t y) {
  return pixels_[y][x];
}

// static
Screen Screen::TerminalFullscreen() {
  Terminal::Dimensions size = Terminal::Size();
  return Screen(size.dimx, size.dimy);
}

// static
Screen Screen::TerminalOutput(std::unique_ptr<dom::Node>& element) {
  element->ComputeRequirement();
  Terminal::Dimensions size = Terminal::Size();
  return Screen(size.dimx, element->requirement().min.y);
}

std::string Screen::ResetPosition() {
  std::stringstream ss;
  for (size_t y = 1; y < dimy_; ++y) {
    ss << "\e[2K\r\e[1A";
  }
  return ss.str();
}

void Screen::Clear() {
  pixels_ = std::vector<std::vector<Pixel>>(dimy_,
                                            std::vector<Pixel>(dimx_, Pixel()));
}

};  // namespace ftxui
