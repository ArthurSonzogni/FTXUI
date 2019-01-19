#ifndef FTXUI_SCREEN_SCREEN
#define FTXUI_SCREEN_SCREEN

#include <string>
#include <vector>
#include <memory>

#include "ftxui/screen/color.hpp"
#include "ftxui/screen/box.hpp"

namespace ftxui {
  class Node;
}

namespace ftxui {

struct Pixel {
  wchar_t character = U' ';
  bool blink = false;
  bool bold = false;
  bool dim = false;
  bool inverted = false;
  bool underlined = false;
  Color background_color = Color::Default;
  Color foreground_color = Color::Default;
};

class Screen {
 public:
  // Constructor.
  Screen(size_t dimx, size_t dimy);

  // Constructor using the terminal.
  static Screen TerminalFullscreen();
  static Screen TerminalOutput(std::unique_ptr<Node>& element);

  // Node write into the screen using Screen::at.
  wchar_t& at(size_t x, size_t y);
  Pixel& PixelAt(size_t x, size_t y);

  // Convert the screen into a printable string in the terminal.
  std::string ToString();

  // Get screen dimensions.
  size_t dimx() { return dimx_;}
  size_t dimy() { return dimy_;}

  // Move the terminal cursor n-lines up with n = dimy().
  std::string ResetPosition();

  // Fill with space.
  void Clear();

  void ApplyShader();
  Box stencil;

 protected:
  size_t dimx_;
  size_t dimy_;
  std::vector<std::vector<Pixel>> pixels_;
};

};  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_SCREEN */
