// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_SCREEN_SCREEN_HPP
#define FTXUI_SCREEN_SCREEN_HPP

#include <cstdint>     // for uint8_t
#include <functional>  // for function
#include <string>      // for string, basic_string, allocator
#include <vector>      // for vector

#include "ftxui/screen/surface.hpp"   // for Surface
#include "ftxui/screen/terminal.hpp"  // for Dimensions
#include "ftxui/util/export.hpp"      // for FTXUI_EXPORT

namespace ftxui {

/// @brief Define how the Screen's dimensions should look like.
/// @ingroup screen
namespace Dimension {
/// Return a fixed dimension `{value, value}`.
FTXUI_EXPORT(SCREEN) Dimensions Fixed(int);
/// Return the current terminal dimensions.
FTXUI_EXPORT(SCREEN) Dimensions Full();
}  // namespace Dimension

/// @brief A rectangular grid of Cell.
/// @ingroup screen
class FTXUI_EXPORT(SCREEN) Screen : public Surface {
 public:
  // Constructors:

  /// Create a screen with dimensions `dimx` x `dimy`.
  Screen(int dimx, int dimy);
  /// Create a screen with the given `dimension`.
  static Screen Create(Dimensions dimension);
  /// Create a screen using `width.dimx` and `height.dimy`.
  static Screen Create(Dimensions width, Dimensions height);

  // Destructor:
  ~Screen() override = default;

  // Copy:
  Screen(const Screen&) = default;
  Screen& operator=(const Screen&) = default;

  /// Serialize the screen content into an ANSI-formatted string.
  std::string ToString() const;
  /// Append the ANSI-formatted screen content into `ss`.
  void ToString(std::string& ss) const;
  /// Append an incremental ANSI diff from `previous` to this screen into `ss`.
  void ToString(std::string& ss, const Screen& previous) const;

  /// Print the screen to standard output.
  void Print() const;

  /// Clear all cells and reset screen state (hyperlinks, cursor).
  void Clear();

  /// Return ANSI escape sequences moving the cursor back to the top-left.
  std::string ResetPosition(bool clear = false) const;
  /// Append ANSI escape sequences moving the cursor back to the top-left.
  void ResetPosition(std::string& ss, bool clear = false) const;

  /// Post-process automerge box-drawing characters across the screen.
  void ApplyShader();

  /// Cursor position and visual shape.
  struct Cursor {
    int x = 0;
    int y = 0;

    enum Shape : uint8_t {
      Hidden = 0,
      BlockBlinking = 1,
      Block = 2,
      UnderlineBlinking = 3,
      Underline = 4,
      BarBlinking = 5,
      Bar = 6,
    };
    Shape shape = Hidden;
  };

  /// Return the current cursor position and shape.
  Cursor cursor() const { return cursor_; }
  /// Set the cursor position and shape.
  void SetCursor(Cursor cursor) { cursor_ = cursor; }

  // ABI Reserve:
  void Reserved1() override;
  void Reserved2() override;
  void Reserved3() override;
  void Reserved4() override;
  void Reserved5() override;
  void Reserved6() override;
  void Reserved7() override;
  void Reserved8() override;

  /// Register a hyperlink URL and return its compact 8-bit ID.
  uint8_t RegisterHyperlink(std::string_view link);
  /// Look up a previously registered hyperlink URL by ID.
  const std::string& Hyperlink(uint8_t id) const;

  using SelectionStyle = std::function<void(Cell&)>;
  /// Return the active text-selection cell stylizer.
  const SelectionStyle& GetSelectionStyle() const;
  /// Set the text-selection cell stylizer.
  void SetSelectionStyle(SelectionStyle decorator);

 protected:
  Cursor cursor_;
  std::vector<std::string> hyperlinks_ = {""};

  // The current selection style. This is overridden by various dom elements.
  SelectionStyle selection_style_ = [](Cell& cell) { cell.inverted ^= true; };
};

}  // namespace ftxui

#endif  // FTXUI_SCREEN_SCREEN_HPP
