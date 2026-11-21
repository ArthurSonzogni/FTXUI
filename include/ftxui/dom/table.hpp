// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_TABLE
#define FTXUI_DOM_TABLE

#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/dom/elements.hpp"  // for Element, BorderStyle, LIGHT, Decorator
#include "ftxui/util/export.hpp"

namespace ftxui {

class Table;
class TableSelection;

/// @brief Table is a utility to draw tables.
///
/// **example**
/// ```cpp
/// auto table = Table({
///  {"X", "Y"},
///  {"-1", "1"},
///  {"+0", "0"},
///  {"+1", "1"},
/// });
///
/// table.SelectAll().Border(LIGHT);
/// table.SelectRow(1).Border(DOUBLE);
/// table.SelectRow(1).SeparatorInternal(LIGHT);
///
/// std::move(table).Render();
/// ```
///
/// @ingroup dom
class FTXUI_EXPORT(DOM) Table {
 public:
  /// Create an empty table.
  Table();
  /// Create a table from a 2D vector of strings.
  explicit Table(const std::vector<std::vector<std::string>>&);
  /// Create a table from a 2D vector of Elements.
  explicit Table(std::vector<std::vector<Element>>);
  /// Create a table from an initializer list of string rows.
  Table(std::initializer_list<std::vector<std::string>> init);
  /// Select all cells in the table.
  TableSelection SelectAll();
  /// Select a single cell at (column, row). Negative indices count from end.
  TableSelection SelectCell(int column, int row);
  /// Select a single row. Negative indices count from the end.
  TableSelection SelectRow(int row_index);
  /// Select an inclusive range of rows [row_min, row_max].
  TableSelection SelectRows(int row_min, int row_max);
  /// Select a single column. Negative indices count from the end.
  TableSelection SelectColumn(int column_index);
  /// Select an inclusive range of columns [column_min, column_max].
  TableSelection SelectColumns(int column_min, int column_max);
  /// Select a rectangular region of cells.
  TableSelection SelectRectangle(int column_min,
                                 int column_max,
                                 int row_min,
                                 int row_max);
  /// Build and return the rendered table Element.
  Element Render();

 private:
  void Initialize(std::vector<std::vector<Element>>);
  friend TableSelection;
  std::vector<std::vector<Element>> elements_;
  int input_dim_x_ = 0;
  int input_dim_y_ = 0;
  int dim_x_ = 0;
  int dim_y_ = 0;
};

/// @brief A rectangular selection of cells in a Table used to apply styling.
/// @ingroup dom
class FTXUI_EXPORT(DOM) TableSelection {
 public:
  // Decorate the whole selection with a decorator.

  /// Apply a decorator to all cells, borders, and separators in the selection.
  void Decorate(const Decorator&);
  /// Apply a decorator to rows where `(row - shift) % modulo == 0`.
  void DecorateAlternateRow(const Decorator&, int modulo = 2, int shift = 0);
  /// Apply a decorator to columns where `(col - shift) % modulo == 0`.
  void DecorateAlternateColumn(const Decorator&, int modulo = 2, int shift = 0);

  // Decorate only the cells of the selection with a decorator.

  /// Apply a decorator to all content cells in the selection.
  void DecorateCells(const Decorator&);
  /// Apply a decorator to content cells on alternating columns.
  void DecorateCellsAlternateColumn(const Decorator&,
                                    int modulo = 2,
                                    int shift = 0);
  /// Apply a decorator to content cells on alternating rows.
  void DecorateCellsAlternateRow(const Decorator&,
                                 int modulo = 2,
                                 int shift = 0);

  // Decorate only the border of the selection with a decorator.

  /// Apply a decorator to all four borders of the selection.
  void DecorateBorder(const Decorator&);
  /// Apply a decorator to the left border of the selection.
  void DecorateBorderLeft(const Decorator&);
  /// Apply a decorator to the right border of the selection.
  void DecorateBorderRight(const Decorator&);
  /// Apply a decorator to the top border of the selection.
  void DecorateBorderTop(const Decorator&);
  /// Apply a decorator to the bottom border of the selection.
  void DecorateBorderBottom(const Decorator&);

  // Decorate only the separator of the selection with a decorator.

  /// Apply a decorator to all internal separators in the selection.
  void DecorateSeparator(const Decorator&);
  /// Apply a decorator to vertical internal separators in the selection.
  void DecorateSeparatorVertical(const Decorator&);
  /// Apply a decorator to horizontal internal separators in the selection.
  void DecorateSeparatorHorizontal(const Decorator&);

  // Decorate the border of the selection with a border style and a decorator.

  /// Draw a border around the selection.
  void Border(BorderStyle border = LIGHT);
  /// Draw a decorated border around the selection.
  void Border(BorderStyle, const Decorator&);
  /// Draw a border on the left side of the selection.
  void BorderLeft(BorderStyle border = LIGHT);
  /// Draw a decorated border on the left side of the selection.
  void BorderLeft(BorderStyle, const Decorator&);
  /// Draw a border on the right side of the selection.
  void BorderRight(BorderStyle border = LIGHT);
  /// Draw a decorated border on the right side of the selection.
  void BorderRight(BorderStyle, const Decorator&);
  /// Draw a border on the top side of the selection.
  void BorderTop(BorderStyle border = LIGHT);
  /// Draw a decorated border on the top side of the selection.
  void BorderTop(BorderStyle, const Decorator&);
  /// Draw a border on the bottom side of the selection.
  void BorderBottom(BorderStyle border = LIGHT);
  /// Draw a decorated border on the bottom side of the selection.
  void BorderBottom(BorderStyle, const Decorator&);

  // Decorate the separator of the selection with a border style and a
  // decorator.

  /// Draw internal horizontal and vertical separator lines.
  void Separator(BorderStyle border = LIGHT);
  /// Draw decorated internal horizontal and vertical separator lines.
  void Separator(BorderStyle, const Decorator&);
  /// Draw internal vertical separator lines.
  void SeparatorVertical(BorderStyle border = LIGHT);
  /// Draw decorated internal vertical separator lines.
  void SeparatorVertical(BorderStyle, const Decorator&);
  /// Draw internal horizontal separator lines.
  void SeparatorHorizontal(BorderStyle border = LIGHT);
  /// Draw decorated internal horizontal separator lines.
  void SeparatorHorizontal(BorderStyle, const Decorator&);

 private:
  friend Table;
  Table* table_;
  int x_min_;
  int x_max_;
  int y_min_;
  int y_max_;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_TABLE */
