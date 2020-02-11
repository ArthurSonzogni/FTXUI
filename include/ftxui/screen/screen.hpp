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

struct Dimension {
  static Dimension Fixed(int);
  static Dimension Fit(std::unique_ptr<Node>&);
  static Dimension Full();

  int dimx;
  int dimy;
};

class Screen {
 public:
  // Constructor.
  Screen(int dimx, int dimy);
  static Screen Create(Dimension dimension);
  static Screen Create(Dimension width, Dimension height);

  // Node write into the screen using Screen::at.
  wchar_t& at(int x, int y);
  Pixel& PixelAt(int x, int y);

  // Convert the screen into a printable string in the terminal.
  std::string ToString();

  // Get screen dimensions.
  int dimx() { return dimx_;}
  int dimy() { return dimy_;}

  // Move the terminal cursor n-lines up with n = dimy().
  std::string ResetPosition();

  // Fill with space.
  void Clear();

  void ApplyShader();
  Box stencil;

  struct Cursor {
    int x = 0;
    int y = 0;
  };
  Cursor cursor() const { return cursor_; }
  void SetCursor(Cursor cursor) { cursor_ = cursor; }

 protected:
  int dimx_;
  int dimy_;
  std::vector<std::vector<Pixel>> pixels_;
  Cursor cursor_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_SCREEN_SCREEN */
