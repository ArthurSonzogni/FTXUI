// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include "ftxui/dom/flexbox_helper.hpp"

#include <algorithm>                     // for max, min
#include <cstddef>                       // for size_t
#include <ftxui/dom/flexbox_config.hpp>  // for FlexboxConfig, FlexboxConfig::Direction, FlexboxConfig::AlignContent, FlexboxConfig::JustifyContent, FlexboxConfig::Wrap, FlexboxConfig::Direction::RowInversed, FlexboxConfig::AlignItems, FlexboxConfig::Direction::Row, FlexboxConfig::Direction::Column, FlexboxConfig::Direction::ColumnInversed, FlexboxConfig::Wrap::WrapInversed, FlexboxConfig::AlignContent::Stretch, FlexboxConfig::JustifyContent::Stretch, FlexboxConfig::Wrap::Wrap, FlexboxConfig::AlignContent::Center, FlexboxConfig::AlignContent::FlexEnd, FlexboxConfig::AlignContent::FlexStart, FlexboxConfig::AlignContent::SpaceAround, FlexboxConfig::AlignContent::SpaceBetween, FlexboxConfig::AlignContent::SpaceEvenly, FlexboxConfig::AlignItems::Center, FlexboxConfig::AlignItems::FlexEnd, FlexboxConfig::AlignItems::FlexStart, FlexboxConfig::AlignItems::Stretch, FlexboxConfig::JustifyContent::Center, FlexboxConfig::JustifyContent::FlexEnd, FlexboxConfig::JustifyContent::FlexStart, FlexboxConfig::JustifyContent::SpaceAround, FlexboxConfig::JustifyContent::SpaceBetween, FlexboxConfig::JustifyContent::SpaceEvenly, FlexboxConfig::Wrap::NoWrap
#include <utility>                       // for swap, move
#include <vector>

#include "ftxui/dom/box_helper.hpp"  // for Element, Compute

namespace ftxui::flexbox_helper {

namespace {
void SymmetryXY(FlexboxConfig& c) {
  std::swap(c.gap_x, c.gap_y);
  switch (c.direction) {
    case FlexboxConfig::Direction::Row:
      c.direction = FlexboxConfig::Direction::Column;
      break;
    case FlexboxConfig::Direction::RowInversed:
      c.direction = FlexboxConfig::Direction::ColumnInversed;
      break;
    case FlexboxConfig::Direction::Column:
      c.direction = FlexboxConfig::Direction::Row;
      break;
    case FlexboxConfig::Direction::ColumnInversed:
      c.direction = FlexboxConfig::Direction::RowInversed;
      break;
  }
}

void SymmetryX(FlexboxConfig& c) {
  switch (c.direction) {
    case FlexboxConfig::Direction::Row:
      c.direction = FlexboxConfig::Direction::RowInversed;
      break;
    case FlexboxConfig::Direction::RowInversed:
      c.direction = FlexboxConfig::Direction::Row;
      break;
    default:
      break;
  }
}

void SymmetryY(FlexboxConfig& c) {
  switch (c.wrap) {
    case FlexboxConfig::Wrap::NoWrap:
      break;
    case FlexboxConfig::Wrap::Wrap:
      c.wrap = FlexboxConfig::Wrap::WrapInversed;
      break;
    case FlexboxConfig::Wrap::WrapInversed:
      c.wrap = FlexboxConfig::Wrap::Wrap;
      break;
  }
}

void SymmetryXY(Global& g) {
  SymmetryXY(g.config);
  std::swap(g.size_x, g.size_y);
  for (auto& b : g.blocks) {
    std::swap(b.min_size_x, b.min_size_y);
    std::swap(b.flex_grow_x, b.flex_grow_y);
    std::swap(b.flex_shrink_x, b.flex_shrink_y);
    std::swap(b.x, b.y);
    std::swap(b.dim_x, b.dim_y);
  }
  for (auto& l : g.lines) {
    std::swap(l.x, l.y);
    std::swap(l.dim_x, l.dim_y);
  }
}

void SymmetryX(Global& g) {
  SymmetryX(g.config);
  for (auto& b : g.blocks) {
    b.x = g.size_x - b.x - b.dim_x;
  }
  for (auto& l : g.lines) {
    l.x = g.size_x - l.x - l.dim_x;
  }
}

void SymmetryY(Global& g) {
  SymmetryY(g.config);
  for (auto& b : g.blocks) {
    b.y = g.size_y - b.y - b.dim_y;
  }
  for (auto& l : g.lines) {
    l.y = g.size_y - l.y - l.dim_y;
  }
}

void SetX(Global& global) {
  for (auto& line : global.lines) {
    std::vector<box_helper::Element> elements;
    elements.reserve(line.blocks.size());
    for (auto* block : line.blocks) {
      box_helper::Element element;
      element.min_size = block->min_size_x;
      element.flex_grow =
          block->flex_grow_x != 0 || global.config.justify_content ==
                                         FlexboxConfig::JustifyContent::Stretch
              ? 1
              : 0;
      element.flex_shrink = block->flex_shrink_x;
      elements.push_back(element);
    }

    box_helper::Compute(
        &elements,
        global.size_x - global.config.gap_x * (int(line.blocks.size()) - 1));

    int x = 0;
    for (size_t i = 0; i < line.blocks.size(); ++i) {
      line.blocks[i]->x = x;
      line.blocks[i]->dim_x = elements[i].size;
      x += elements[i].size;
      x += global.config.gap_x;
    }
  }

  for (auto& line : global.lines) {
    line.x = 0;
    line.dim_x = global.size_x;
  }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void SetY(Global& g) {
  std::vector<box_helper::Element> elements;
  elements.reserve(g.lines.size());
  for (auto& line : g.lines) {
    box_helper::Element element;
    element.flex_shrink = line.blocks.front()->flex_shrink_y;
    element.flex_grow = line.blocks.front()->flex_grow_y;
    for (auto* block : line.blocks) {
      element.min_size = std::max(element.min_size, block->min_size_y);
      element.flex_shrink = std::min(element.flex_shrink, block->flex_shrink_y);
      element.flex_grow = std::min(element.flex_grow, block->flex_grow_y);
    }
    elements.push_back(element);
  }

  // box_helper::Compute(&elements, g.size_y);
  box_helper::Compute(&elements, 10000);  // NOLINT

  // [Align-content]
  std::vector<int> ys(elements.size());
  int y = 0;
  for (size_t i = 0; i < elements.size(); ++i) {
    ys[i] = y;
    y += elements[i].size;
    y += g.config.gap_y;
  }
  int remaining_space = std::max(0, g.size_y - y);
  switch (g.config.align_content) {
    case FlexboxConfig::AlignContent::FlexStart: {
      break;
    }

    case FlexboxConfig::AlignContent::FlexEnd: {
      for (size_t i = 0; i < ys.size(); ++i) {  // NOLINT
        ys[i] += remaining_space;
      }
      break;
    }

    case FlexboxConfig::AlignContent::Center: {
      for (size_t i = 0; i < ys.size(); ++i) {  // NOLINT
        ys[i] += remaining_space / 2;
      }
      break;
    }

    case FlexboxConfig::AlignContent::Stretch: {
      for (int i = static_cast<int>(ys.size()) - 1; i >= 0; --i) {  // NOLINT
        const int shifted = remaining_space * (i + 0) / (i + 1);
        ys[i] += shifted;
        const int consumed = remaining_space - shifted;
        elements[i].size += consumed;
        remaining_space -= consumed;
      }
      break;
    }

    case FlexboxConfig::AlignContent::SpaceBetween: {
      for (int i = static_cast<int>(ys.size()) - 1; i >= 1; --i) {  // NOLINT
        ys[i] += remaining_space;
        remaining_space = remaining_space * (i - 1) / i;
      }
      break;
    }

    case FlexboxConfig::AlignContent::SpaceAround: {
      for (int i = static_cast<int>(ys.size()) - 1; i >= 0; --i) {  // NOLINT
        ys[i] += remaining_space * (2 * i + 1) / (2 * i + 2);
        remaining_space = remaining_space * (2 * i) / (2 * i + 2);
      }
      break;
    }

    case FlexboxConfig::AlignContent::SpaceEvenly: {
      for (int i = static_cast<int>(ys.size()) - 1; i >= 0; --i) {  // NOLINT
        ys[i] += remaining_space * (i + 1) / (i + 2);
        remaining_space = remaining_space * (i + 1) / (i + 2);
      }
      break;
    }
  }

  // [Align items]
  for (size_t i = 0; i < g.lines.size(); ++i) {
    auto& element = elements[i];
    for (auto* block : g.lines[i].blocks) {
      const bool stretch =
          block->flex_grow_y != 0 ||
          g.config.align_content == FlexboxConfig::AlignContent::Stretch;
      const int size =
          stretch ? element.size : std::min(element.size, block->min_size_y);
      switch (g.config.align_items) {
        case FlexboxConfig::AlignItems::FlexStart: {
          block->y = ys[i];
          block->dim_y = size;
          break;
        }

        case FlexboxConfig::AlignItems::Center: {
          block->y = ys[i] + (element.size - size) / 2;
          block->dim_y = size;
          break;
        }

        case FlexboxConfig::AlignItems::FlexEnd: {
          block->y = ys[i] + element.size - size;
          block->dim_y = size;
          break;
        }

        case FlexboxConfig::AlignItems::Stretch: {
          block->y = ys[i];
          block->dim_y = element.size;
          break;
        }
      }
    }
  }

  ys.push_back(g.size_y);
  for (size_t i = 0; i < g.lines.size(); ++i) {
    g.lines[i].y = ys[i];
    g.lines[i].dim_y = ys[i + 1] - ys[i];
  }
}

void JustifyContent(Global& g) {
  for (auto& line : g.lines) {
    Block* last = line.blocks.back();
    int remaining_space = g.size_x - last->x - last->dim_x;
    switch (g.config.justify_content) {
      case FlexboxConfig::JustifyContent::FlexStart:
      case FlexboxConfig::JustifyContent::Stretch:
        break;

      case FlexboxConfig::JustifyContent::FlexEnd: {
        for (auto* block : line.blocks) {
          block->x += remaining_space;
        }
        break;
      }

      case FlexboxConfig::JustifyContent::Center: {
        for (auto* block : line.blocks) {
          block->x += remaining_space / 2;
        }
        break;
      }

      case FlexboxConfig::JustifyContent::SpaceBetween: {
        for (int i = (int)line.blocks.size() - 1; i >= 1; --i) {
          line.blocks[i]->x += remaining_space;
          remaining_space = remaining_space * (i - 1) / i;
        }
        break;
      }

      case FlexboxConfig::JustifyContent::SpaceAround: {
        for (int i = (int)line.blocks.size() - 1; i >= 0; --i) {
          line.blocks[i]->x += remaining_space * (2 * i + 1) / (2 * i + 2);
          remaining_space = remaining_space * (2 * i) / (2 * i + 2);
        }
        break;
      }

      case FlexboxConfig::JustifyContent::SpaceEvenly: {
        for (int i = (int)line.blocks.size() - 1; i >= 0; --i) {
          line.blocks[i]->x += remaining_space * (i + 1) / (i + 2);
          remaining_space = remaining_space * (i + 1) / (i + 2);
        }
        break;
      }
    }
  }
}

void Compute1(Global& global);
void Compute2(Global& global);
void Compute3(Global& global);

void Compute1(Global& global) {
  if (global.config.direction == FlexboxConfig::Direction::RowInversed) {
    SymmetryX(global);
    Compute2(global);
    SymmetryX(global);
    return;
  }
  Compute2(global);
}

void Compute2(Global& global) {
  if (global.config.wrap == FlexboxConfig::Wrap::WrapInversed) {
    SymmetryY(global);
    Compute3(global);
    SymmetryY(global);
    return;
  }
  Compute3(global);
}

void Compute3(Global& global) {
  // Step 1: Lay out every elements into rows:
  {
    Line line;
    int x = 0;
    for (auto& block : global.blocks) {
      // Does it fit the end of the row?
      // No? Then we need to start a new one:
      if (x + block.min_size_x > global.size_x) {
        x = 0;
        if (!line.blocks.empty()) {
          global.lines.push_back(std::move(line));
        }
        line = Line();
      }

      block.line = static_cast<int>(global.lines.size());
      block.line_position = static_cast<int>(line.blocks.size());
      line.blocks.push_back(&block);
      x += block.min_size_x + global.config.gap_x;
    }
    if (!line.blocks.empty()) {
      global.lines.push_back(std::move(line));
    }
  }

  // Step 2: Set positions on the X axis.
  SetX(global);
  JustifyContent(global);  // Distribute remaining space.

  // Step 3: Set positions on the Y axis.
  SetY(global);
}

}  // namespace

void Compute(Global& global) {
  if (global.config.direction == FlexboxConfig::Direction::Column ||
      global.config.direction == FlexboxConfig::Direction::ColumnInversed) {
    SymmetryXY(global);
    Compute1(global);
    SymmetryXY(global);
    return;
  }
  Compute1(global);
}

}  // namespace ftxui::flexbox_helper
