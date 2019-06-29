#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/screen/terminal.hpp"

#include <sstream>

namespace ftxui {

namespace {
static const wchar_t* BOLD_SET = L"\e[1m";
static const wchar_t* BOLD_RESET = L"\e[22m";  // Can't use 21 here.

static const wchar_t* DIM_SET = L"\e[2m";
static const wchar_t* DIM_RESET = L"\e[22m";

static const wchar_t* UNDERLINED_SET = L"\e[4m";
static const wchar_t* UNDERLINED_RESET = L"\e[24m";

static const wchar_t* BLINK_SET = L"\e[5m";
static const wchar_t* BLINK_RESET = L"\e[25m";

static const wchar_t* INVERTED_SET = L"\e[7m";
static const wchar_t* INVERTED_RESET = L"\e[27m";

static const char* MOVE_LEFT = "\r";
static const char* MOVE_UP = "\e[1A";
static const char* CLEAR_LINE = "\e[2K";

bool In(const Box& stencil, int x, int y) {
  return stencil.x_min <= x && x <= stencil.x_max &&  //
         stencil.y_min <= y && y <= stencil.y_max;
}

Pixel dev_null_pixel;

}  // namespace

Dimension Dimension::Fixed(int v) {
  return Dimension{v, v};
}

Dimension Dimension::Fit(std::unique_ptr<Node>& e) {
  e->ComputeRequirement();
  Terminal::Dimensions size = Terminal::Size();
  return Dimension{std::min(e->requirement().min.x, size.dimx),
                   std::min(e->requirement().min.y, size.dimy)};
}

Dimension Dimension::Full() {
  Terminal::Dimensions size = Terminal::Size();
  return Dimension{size.dimx, size.dimy};
}

// static
Screen Screen::Create(Dimension width, Dimension height) {
  return Screen(width.dimx, height.dimy);
}

// static
Screen Screen::Create(Dimension dimension) {
  return Screen(dimension.dimx, dimension.dimy);
}

Screen::Screen(int dimx, int dimy)
    : stencil({0, dimx - 1, 0, dimy - 1}),
      dimx_(dimx),
      dimy_(dimy),
      pixels_(dimy, std::vector<Pixel>(dimx)) {}

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
    ss << L"\e[" + to_wstring(std::to_string((uint8_t)next.foreground_color)) +
              L"m";
    ss << L"\e[" +
              to_wstring(std::to_string(10 + (uint8_t)next.background_color)) +
              L"m";
  }

  previous = next;
}

std::string Screen::ToString() {
  std::wstringstream ss;

  Pixel previous_pixel;

  for (int y = 0; y < dimy_; ++y) {
    if (y != 0)
      ss << '\n';
    for (int x = 0; x < dimx_;) {
      auto& pixel = pixels_[y][x];
      wchar_t c = pixel.character;
      UpdatePixelStyle(ss, previous_pixel, pixel);
      ss << c;
      x += wchar_width(c);
    }
  }

  Pixel final_pixel;
  UpdatePixelStyle(ss, previous_pixel, final_pixel);

  return to_string(ss.str());
}

wchar_t& Screen::at(int x, int y) {
  return PixelAt(x, y).character;
}

Pixel& Screen::PixelAt(int x, int y) {
  return In(stencil, x, y) ? pixels_[y][x] : dev_null_pixel;
}

std::string Screen::ResetPosition() {
  std::stringstream ss;
  ss << MOVE_LEFT << CLEAR_LINE;
  for (int y = 1; y < dimy_; ++y) {
    ss << MOVE_UP << CLEAR_LINE;
  }
  return ss.str();
}

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

};  // namespace ftxui
