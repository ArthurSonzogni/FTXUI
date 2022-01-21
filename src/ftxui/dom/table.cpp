#include "ftxui/dom/table.hpp"

#include <algorithm>  // for max
#include <memory>   // for allocator, shared_ptr, allocator_traits<>::value_type
#include <utility>  // for move, swap

#include "ftxui/dom/elements.hpp"  // for Element, operator|, text, separatorCharacter, Elements, BorderStyle, Decorator, emptyElement, size, gridbox, EQUAL, flex, flex_shrink, HEIGHT, WIDTH

namespace ftxui {
namespace {

bool IsCell(int x, int y) {
  return x % 2 == 1 && y % 2 == 1;
}

static std::string charset[6][6] = {
    {"┌", "┐", "└", "┘", "─", "│"},  //
    {"┏", "┓", "┗", "┛", "━", "┃"},  //
    {"╔", "╗", "╚", "╝", "═", "║"},  //
    {"╭", "╮", "╰", "╯", "─", "│"},  //
    {" ", " ", " ", " ", " ", " "},  //
};

int Wrap(int input, int modulo) {
  input %= modulo;
  input += modulo;
  input %= modulo;
  return input;
}

void Order(int& a, int& b) {
  if (a >= b)
    std::swap(a, b);
}

}  // namespace

Table::Table() {
  Initialize({});
}

Table::Table(std::vector<std::vector<std::string>> input) {
  std::vector<std::vector<Element>> output;
  for(auto& row : input) {
    output.push_back({});
    auto& output_row = output.back();
    for(auto& cell : row) {
      output_row.push_back(text(cell));
    }
  }
  Initialize(std::move(output));
}

Table::Table(std::vector<std::vector<Element>> input) {
  Initialize(std::move(input));
}

void Table::Initialize(std::vector<std::vector<Element>> input) {
  input_dim_y_ = input.size();
  input_dim_x_ = 0;
  for (auto& row : input)
    input_dim_x_ = std::max(input_dim_x_, (int)row.size());

  dim_y_ = 2 * input_dim_y_ + 1;
  dim_x_ = 2 * input_dim_x_ + 1;

  // Reserve space.
  elements_.resize(dim_y_);
  for (int y = 0; y < dim_y_; ++y)
    elements_[y].resize(dim_x_);

  // Transfert elements_ from |input| toward |elements_|.
  {
    int y = 1;
    for (auto& row : input) {
      int x = 1;
      for (auto& cell : row) {
        elements_[y][x] = std::move(cell);
        x += 2;
      }
      y += 2;
    }
  }

  // Add empty element for the border.
  for (int y = 0; y < dim_y_; ++y) {
    for (int x = 0; x < dim_x_; ++x) {
      auto& element = elements_[y][x];

      if (IsCell(x, y)) {
        if (!element)
          element = emptyElement();
        continue;
      }

      element = emptyElement();
    }
  }
}

TableSelection Table::SelectRow(int index) {
  return SelectRectangle(0, -1, index, index);
}

TableSelection Table::SelectRows(int row_min, int row_max) {
  return SelectRectangle(0, -1, row_min, row_max);
}

TableSelection Table::SelectColumn(int index) {
  return SelectRectangle(index, index, 0, -1);
}

TableSelection Table::SelectColumns(int column_min, int column_max) {
  return SelectRectangle(column_min, column_max, 0, -1);
}

TableSelection Table::SelectCell(int column, int row) {
  return SelectRectangle(column, column, row, row);
}

TableSelection Table::SelectRectangle(int column_min,
                                      int column_max,
                                      int row_min,
                                      int row_max) {
  column_min = Wrap(column_min, input_dim_x_);
  column_max = Wrap(column_max, input_dim_x_);
  Order(column_min, column_max);
  row_min = Wrap(row_min, input_dim_y_);
  row_max = Wrap(row_max, input_dim_y_);
  Order(row_min, row_max);

  TableSelection output;
  output.table_ = this;
  output.x_min_ = 2 * column_min;
  output.x_max_ = 2 * column_max + 2;
  output.y_min_ = 2 * row_min;
  output.y_max_ = 2 * row_max + 2;
  return output;
}

TableSelection Table::SelectAll() {
  TableSelection output;
  output.table_ = this;
  output.x_min_ = 0;
  output.x_max_ = dim_x_ - 1;
  output.y_min_ = 0;
  output.y_max_ = dim_y_ - 1;
  return output;
}

Element Table::Render() {
  for (int y = 0; y < dim_y_; ++y) {
    for (int x = 0; x < dim_x_; ++x) {
      auto& it = elements_[y][x];

      // Line
      if ((x + y) % 2 == 1) {
        it = std::move(it) | flex;
        continue;
      }

      // Cells
      if ((x % 2) == 1 && (y % 2) == 1) {
        it = std::move(it) | flex_shrink;
        continue;
      }

      // Corners
      it = std::move(it) | size(WIDTH, EQUAL, 0) | size(HEIGHT, EQUAL, 0);
    }
  }
  return gridbox(std::move(elements_));
}

void TableSelection::Decorate(Decorator decorator) {
  for (int y = y_min_; y <= y_max_; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      Element& e = table_->elements_[y][x];
      e = std::move(e) | decorator;
    }
  }
}

void TableSelection::DecorateCells(Decorator decorator) {
  for (int y = y_min_; y <= y_max_; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      if (y % 2 && x % 2) {
        Element& e = table_->elements_[y][x];
        e = std::move(e) | decorator;
      }
    }
  }
}

void TableSelection::DecorateAlternateColumn(Decorator decorator,
                                             int modulo,
                                             int shift) {
  for (int y = y_min_; y <= y_max_; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      if (y % 2 && (x / 2) % modulo == shift) {
        Element& e = table_->elements_[y][x];
        e = std::move(e) | decorator;
      }
    }
  }
}

void TableSelection::DecorateAlternateRow(Decorator decorator,
                                          int modulo,
                                          int shift) {
  for (int y = y_min_ + 1; y <= y_max_ - 1; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      if (y % 2 && (y / 2) % modulo == shift) {
        Element& e = table_->elements_[y][x];
        e = std::move(e) | decorator;
      }
    }
  }
}

void TableSelection::DecorateCellsAlternateColumn(Decorator decorator,
                                                  int modulo,
                                                  int shift) {
  for (int y = y_min_; y <= y_max_; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      if (y % 2 && x % 2 && ((x / 2) % modulo == shift)) {
        Element& e = table_->elements_[y][x];
        e = std::move(e) | decorator;
      }
    }
  }
}

void TableSelection::DecorateCellsAlternateRow(Decorator decorator,
                                               int modulo,
                                               int shift) {
  for (int y = y_min_; y <= y_max_; ++y) {
    for (int x = x_min_; x <= x_max_; ++x) {
      if (y % 2 && x % 2 && ((y / 2) % modulo == shift)) {
        Element& e = table_->elements_[y][x];
        e = std::move(e) | decorator;
      }
    }
  }
}

void TableSelection::Border(BorderStyle style) {
  BorderLeft(style);
  BorderRight(style);
  BorderTop(style);
  BorderBottom(style);

  table_->elements_[y_min_][x_min_] = text(charset[style][0]);
  table_->elements_[y_min_][x_max_] = text(charset[style][1]);
  table_->elements_[y_max_][x_min_] = text(charset[style][2]);
  table_->elements_[y_max_][x_max_] = text(charset[style][3]);
}

void TableSelection::Separator(BorderStyle style) {
  for (int y = y_min_ + 1; y <= y_max_ - 1; ++y) {
    for (int x = x_min_ + 1; x <= x_max_ - 1; ++x) {
      if (y % 2 == 0 || x % 2 == 0) {
        Element& e = table_->elements_[y][x];
        e = (y % 2) ? separatorCharacter(charset[style][5])
                    : separatorCharacter(charset[style][4]);
      }
    }
  }
}

void TableSelection::SeparatorVertical(BorderStyle style) {
  for (int y = y_min_ + 1; y <= y_max_ - 1; ++y) {
    for (int x = x_min_ + 1; x <= x_max_ - 1; ++x) {
      if (x % 2 == 0) {
        table_->elements_[y][x] = separatorCharacter(charset[style][5]);
      }
    }
  }
}

void TableSelection::SeparatorHorizontal(BorderStyle style) {
  for (int y = y_min_ + 1; y <= y_max_ - 1; ++y) {
    for (int x = x_min_ + 1; x <= x_max_ - 1; ++x) {
      if (y % 2 == 0) {
        table_->elements_[y][x] = separatorCharacter(charset[style][4]);
      }
    }
  }
}

void TableSelection::BorderLeft(BorderStyle style) {
  for (int y = y_min_; y <= y_max_; y++)
    table_->elements_[y][x_min_] = separatorCharacter(charset[style][5]);
}

void TableSelection::BorderRight(BorderStyle style) {
  for (int y = y_min_; y <= y_max_; y++)
    table_->elements_[y][x_max_] = separatorCharacter(charset[style][5]);
}

void TableSelection::BorderTop(BorderStyle style) {
  for (int x = x_min_; x <= x_max_; x++)
    table_->elements_[y_min_][x] = separatorCharacter(charset[style][4]);
}

void TableSelection::BorderBottom(BorderStyle style) {
  for (int x = x_min_; x <= x_max_; x++)
    table_->elements_[y_max_][x] = separatorCharacter(charset[style][4]);
}

}  // namespace ftxui

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
