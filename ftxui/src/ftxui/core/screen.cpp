#include "ftxui/core/screen.hpp"
#include "ftxui/core/terminal.hpp"
#include "ftxui/util/string.hpp"

#include <sstream>

namespace ftxui {

Screen::Screen(size_t dimx, size_t dimy)
    : dimx_(dimx), dimy_(dimy), lines_(dimy, std::wstring(dimx, U' ')) {}

std::string Screen::ToString() {
  std::stringstream ss;
  for (size_t y = 0; y < dimy_; ++y) {
    ss << to_string(lines_[y]);
    if (y + 1 < dimy_)
      ss << '\n';
  }
  return ss.str();
}

wchar_t& Screen::at(size_t x, size_t y) {
  return lines_[y][x];
}

Screen Screen::WholeTerminal() {
  Terminal::Dimensions size = Terminal::Size();
  return Screen(size.dimx, size.dimy);
}

};  // namespace ftxui
