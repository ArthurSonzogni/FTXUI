// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_DOM_TABLE
#define FTXUI_DOM_TABLE

#include <string>  // for string
#include <vector>  // for vector

#include "ftxui/dom/elements.hpp"  // for Element, BorderStyle, LIGHT, Decorator

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
class Table {
 public:
  Table();
  explicit Table(std::vector<std::vector<std::string>>);
  explicit Table(std::vector<std::vector<Element>>);
  Table(std::initializer_list<std::vector<std::string>> init);
  TableSelection SelectAll();
  TableSelection SelectCell(int column, int row);
  TableSelection SelectRow(int row_index);
  TableSelection SelectRows(int row_min, int row_max);
  TableSelection SelectColumn(int column_index);
  TableSelection SelectColumns(int column_min, int column_max);
  TableSelection SelectRectangle(int column_min,
                                 int column_max,
                                 int row_min,
                                 int row_max);
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

class TableSelection {
 public:
  // Decorate the whole selection with a decorator.
  void Decorate(Decorator);
  void DecorateAlternateRow(Decorator, int modulo = 2, int shift = 0);
  void DecorateAlternateColumn(Decorator, int modulo = 2, int shift = 0);

  // Decorate only the cells of the selection with a decorator.
  void DecorateCells(Decorator);
  void DecorateCellsAlternateColumn(Decorator, int modulo = 2, int shift = 0);
  void DecorateCellsAlternateRow(Decorator, int modulo = 2, int shift = 0);

  // Decorate only the border of the selection with a decorator.
  void DecorateBorder(Decorator);
  void DecorateBorderLeft(Decorator);
  void DecorateBorderRight(Decorator);
  void DecorateBorderTop(Decorator);
  void DecorateBorderBottom(Decorator);

  // Decorate only the separator of the selection with a decorator.
  void DecorateSeparator(Decorator);
  void DecorateSeparatorVertical(Decorator);
  void DecorateSeparatorHorizontal(Decorator);

  // Decorate the border of the selection with a border style and a decorator.
  void Border(BorderStyle border = LIGHT);
  void Border(BorderStyle, Decorator);
  void BorderLeft(BorderStyle border = LIGHT);
  void BorderLeft(BorderStyle, Decorator);
  void BorderRight(BorderStyle border = LIGHT);
  void BorderRight(BorderStyle, Decorator);
  void BorderTop(BorderStyle border = LIGHT);
  void BorderTop(BorderStyle, Decorator);
  void BorderBottom(BorderStyle border = LIGHT);
  void BorderBottom(BorderStyle, Decorator);

  // Decorate the separator of the selection with a border style and a
  // decorator.
  void Separator(BorderStyle border = LIGHT);
  void Separator(BorderStyle, Decorator);
  void SeparatorVertical(BorderStyle border = LIGHT);
  void SeparatorVertical(BorderStyle, Decorator);
  void SeparatorHorizontal(BorderStyle border = LIGHT);
  void SeparatorHorizontal(BorderStyle, Decorator);

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
