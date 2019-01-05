#include "ftxui/screen.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/terminal.hpp"
#include "ftxui/util/string.hpp"

#include <sstream>

namespace ftxui {

Screen::Screen(size_t dimx, size_t dimy)
    : dimx_(dimx), dimy_(dimy), pixels_(dimy, std::vector<Pixel>(dimx)) {}

void UpdatePixelStyle(std::wstringstream& ss, Pixel& previous, Pixel& next) {
  if (next.bold != previous.bold)
    ss << (next.bold ? L"\e[1m" : L"\e[22m"); // Can't use 21 here.
  if (next.dim != previous.dim)
    ss << (next.dim ? L"\e[2m" : L"\e[22m");
  if (next.underlined != previous.underlined)
    ss << (next.underlined ? L"\e[4m" : L"\e[24m");
  if (next.blink != previous.blink)
    ss << (next.blink ? L"\e[5m" : L"\e[25m");
  if (next.inverted != previous.inverted)
    ss << (next.inverted ? L"\e[7m" : L"\e[27m");
  if (next.foreground_color != previous.foreground_color ||
      next.background_color != previous.background_color) {
    ss << L"\e[" + to_wstring(std::to_string((uint8_t)next.foreground_color)) + L"m";
    ss << L"\e[" + to_wstring(std::to_string(10 + (uint8_t)next.background_color)) + L"m";
  }

  previous = next;
}

std::string Screen::ToString() {
  std::wstringstream ss;

  Pixel previous_pixel;

  for (size_t y = 0; y < dimy_; ++y) {
    for (size_t x = 0; x < dimx_; ++x) {
      UpdatePixelStyle(ss, previous_pixel, pixels_[y][x]);
      ss << pixels_[y][x].character;
    }

    if (y + 1 < dimy_)
      ss << '\n';
    Pixel final_pixel;
    UpdatePixelStyle(ss, previous_pixel, final_pixel);
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
