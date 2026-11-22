// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_CANVAS_HPP
#define FTXUI_DOM_CANVAS_HPP

#include <cstddef>        // for size_t
#include <functional>     // for function
#include <string>         // for string
#include <unordered_map>  // for unordered_map

#include "ftxui/screen/color.hpp"    // for Color
#include "ftxui/screen/surface.hpp"  // for Cell, Surface
#include "ftxui/util/export.hpp"

#ifdef DrawText
// Workaround for WinUsr.h (via Windows.h) defining macros that break things.
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-drawtext
#undef DrawText
#endif

namespace ftxui {

/// @brief Canvas is a drawable buffer associated with drawing operations.
///
/// Canvas is a drawable area that can be used to create complex graphics. It
/// supports drawing points, lines, circles, ellipses, text, and images using
/// braille, block, or normal characters.
///
/// Note: A terminal contains cells. A cells is a unit of:
/// - 2x4 braille characters (1x1 pixel)
/// - 2x2 block characters (2x2 pixels)
/// - 2x4 normal characters (2x4 pixels)
///
/// You need to multiply the x coordinate by 2 and the y coordinate by 4 to
/// get the correct position in the terminal.
///
/// @ingroup dom
struct FTXUI_EXPORT(DOM) Canvas {
 public:
  Canvas() = default;
  /// Create a canvas of size `width` x `height` (2x4 braille dots per cell).
  Canvas(int width, int height);

  // Getters:

  /// Width of the canvas in braille dots (2 per cell).
  int width() const { return width_; }
  /// Height of the canvas in braille dots (4 per cell).
  int height() const { return height_; }
  /// Return the cell at terminal coordinate (x, y).
  Cell GetCell(int x, int y) const;
  /// Deprecated alias for GetCell().
  Cell GetPixel(int x, int y) const { return GetCell(x, y); }

  using Stylizer = std::function<void(Cell&)>;

  // Draws using braille characters --------------------------------------------

  /// Turn on the braille dot at (x, y).
  void DrawPointOn(int x, int y);
  /// Turn off the braille dot at (x, y).
  void DrawPointOff(int x, int y);
  /// Toggle the braille dot at (x, y).
  void DrawPointToggle(int x, int y);
  /// Set the braille dot at (x, y) to `value`.
  void DrawPoint(int x, int y, bool value);
  /// Set the braille dot at (x, y) to `value` and apply `s`.
  void DrawPoint(int x, int y, bool value, const Stylizer& s);
  /// Set the braille dot at (x, y) to `value` with foreground `color`.
  void DrawPoint(int x, int y, bool value, const Color& color);
  /// Draw a braille line from (x1, y1) to (x2, y2).
  void DrawPointLine(int x1, int y1, int x2, int y2);
  /// Draw a styled braille line from (x1, y1) to (x2, y2).
  void DrawPointLine(int x1, int y1, int x2, int y2, const Stylizer& s);
  /// Draw a colored braille line from (x1, y1) to (x2, y2).
  void DrawPointLine(int x1, int y1, int x2, int y2, const Color& color);
  /// Draw a braille circle outline centered at (x, y).
  void DrawPointCircle(int x, int y, int radius);
  /// Draw a styled braille circle outline centered at (x, y).
  void DrawPointCircle(int x, int y, int radius, const Stylizer& s);
  /// Draw a colored braille circle outline centered at (x, y).
  void DrawPointCircle(int x, int y, int radius, const Color& color);
  /// Draw a filled braille circle centered at (x, y).
  void DrawPointCircleFilled(int x, int y, int radius);
  /// Draw a styled filled braille circle centered at (x, y).
  void DrawPointCircleFilled(int x, int y, int radius, const Stylizer& s);
  /// Draw a colored filled braille circle centered at (x, y).
  void DrawPointCircleFilled(int x, int y, int radius, const Color& color);
  /// Draw a braille ellipse outline centered at (x, y) with radii (r1, r2).
  void DrawPointEllipse(int x, int y, int r1, int r2);
  /// Draw a colored braille ellipse outline centered at (x, y).
  void DrawPointEllipse(int x, int y, int r1, int r2, const Color& color);
  /// Draw a styled braille ellipse outline centered at (x, y).
  void DrawPointEllipse(int x, int y, int r1, int r2, const Stylizer& s);
  /// Draw a filled braille ellipse centered at (x, y) with radii (r1, r2).
  void DrawPointEllipseFilled(int x, int y, int r1, int r2);
  /// Draw a colored filled braille ellipse centered at (x, y).
  void DrawPointEllipseFilled(int x, int y, int r1, int r2, const Color& color);
  /// Draw a styled filled braille ellipse centered at (x, y).
  void DrawPointEllipseFilled(int x, int y, int r1, int r2, const Stylizer& s);

  // Draw using box characters -------------------------------------------------
  // Blocks are of size 1x2. y is considered to be a multiple of 2.

  /// Turn on the half-block pixel at (x, y).
  void DrawBlockOn(int x, int y);
  /// Turn off the half-block pixel at (x, y).
  void DrawBlockOff(int x, int y);
  /// Toggle the half-block pixel at (x, y).
  void DrawBlockToggle(int x, int y);
  /// Set the half-block pixel at (x, y) to `value`.
  void DrawBlock(int x, int y, bool value);
  /// Set the half-block pixel at (x, y) to `value` and apply `s`.
  void DrawBlock(int x, int y, bool value, const Stylizer& s);
  /// Set the half-block pixel at (x, y) to `value` with foreground `color`.
  void DrawBlock(int x, int y, bool value, const Color& color);
  /// Draw a block line from (x1, y1) to (x2, y2).
  void DrawBlockLine(int x1, int y1, int x2, int y2);
  /// Draw a styled block line from (x1, y1) to (x2, y2).
  void DrawBlockLine(int x1, int y1, int x2, int y2, const Stylizer& s);
  /// Draw a colored block line from (x1, y1) to (x2, y2).
  void DrawBlockLine(int x1, int y1, int x2, int y2, const Color& color);
  /// Draw a block circle outline centered at (x1, y1).
  void DrawBlockCircle(int x1, int y1, int radius);
  /// Draw a styled block circle outline centered at (x1, y1).
  void DrawBlockCircle(int x1, int y1, int radius, const Stylizer& s);
  /// Draw a colored block circle outline centered at (x1, y1).
  void DrawBlockCircle(int x1, int y1, int radius, const Color& color);
  /// Draw a filled block circle centered at (x1, y1).
  void DrawBlockCircleFilled(int x1, int y1, int radius);
  /// Draw a styled filled block circle centered at (x1, y1).
  void DrawBlockCircleFilled(int x1, int y1, int radius, const Stylizer& s);
  /// Draw a colored filled block circle centered at (x1, y1).
  void DrawBlockCircleFilled(int x1, int y1, int radius, const Color& color);
  /// Draw a block ellipse outline centered at (x1, y1) with radii (r1, r2).
  void DrawBlockEllipse(int x1, int y1, int r1, int r2);
  /// Draw a styled block ellipse outline centered at (x1, y1).
  void DrawBlockEllipse(int x1, int y1, int r1, int r2, const Stylizer& s);
  /// Draw a colored block ellipse outline centered at (x1, y1).
  void DrawBlockEllipse(int x1, int y1, int r1, int r2, const Color& color);
  /// Draw a filled block ellipse centered at (x1, y1) with radii (r1, r2).
  void DrawBlockEllipseFilled(int x1, int y1, int r1, int r2);
  /// Draw a styled filled block ellipse centered at (x1, y1).
  void DrawBlockEllipseFilled(int x1,
                              int y1,
                              int r1,
                              int r2,
                              const Stylizer& s);
  /// Draw a colored filled block ellipse centered at (x1, y1).
  void DrawBlockEllipseFilled(int x1,
                              int y1,
                              int r1,
                              int r2,
                              const Color& color);

  // Draw using normal characters ----------------------------------------------
  // Draw using character of size 2x4 at position (x,y)
  // x is considered to be a multiple of 2.
  // y is considered to be a multiple of 4.

  /// Draw UTF-8 text starting at canvas position (x, y).
  void DrawText(int x, int y, std::string_view value);
  /// Draw colored UTF-8 text starting at canvas position (x, y).
  void DrawText(int x, int y, std::string_view value, const Color& color);
  /// Draw styled UTF-8 text starting at canvas position (x, y).
  void DrawText(int x, int y, std::string_view value, const Stylizer& style);

  // Draw using directly pixels or images --------------------------------------
  // x is considered to be a multiple of 2.
  // y is considered to be a multiple of 4.

  /// Directly write a Cell at canvas position (x, y).
  void DrawCell(int x, int y, const Cell&);
  /// Blit a Surface onto the canvas with its top-left corner at (x, y).
  void DrawSurface(int x, int y, const Surface&);

  /// Deprecated alias for DrawCell().
  void DrawPixel(int x, int y, const Cell& cell) { DrawCell(x, y, cell); }
  /// Deprecated alias for DrawSurface().
  void DrawImage(int x, int y, const Surface& s) { DrawSurface(x, y, s); }

  // Decorator:
  // x is considered to be a multiple of 2.
  // y is considered to be a multiple of 4.

  /// Apply a Stylizer callback to the cell at canvas position (x, y).
  void Style(int x, int y, const Stylizer& style);

 private:
  friend class CanvasNodeBase;

  bool IsIn(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
  }

  enum CellType {
    kCell,     // Units of size 2x4
    kBlock,    // Units of size 2x2
    kBraille,  // Units of size 1x1
  };

  struct CanvasCell {
    CellType type = kCell;
    Cell content;
  };

  struct XY {
    int x;
    int y;
    bool operator==(const XY& other) const {
      return x == other.x && y == other.y;
    }
  };

  struct XYHash {
    size_t operator()(const XY& xy) const {
      constexpr size_t shift = 1024;
      return size_t(xy.x) * shift + size_t(xy.y);
    }
  };

  int width_ = 0;
  int height_ = 0;
  std::unordered_map<XY, CanvasCell, XYHash> storage_;
};

}  // namespace ftxui

#endif  // FTXUI_DOM_CANVAS_HPP
