#include <stddef.h>   // for size_t
#include <algorithm>  // for max, min
#include <memory>  // for __shared_ptr_access, shared_ptr, make_shared, allocator_traits<>::value_type
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type

#include "ftxui/dom/box_helper.hpp"   // for Element, Compute
#include "ftxui/dom/elements.hpp"     // for Elements, filler, Element, gridbox
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {
class Screen;

class GridBox : public Node {
 public:
  GridBox(std::vector<Elements> lines) : Node(), lines_(std::move(lines)) {
    y_size = lines_.size();
    for (const auto& line : lines_)
      x_size = std::max(x_size, (int)line.size());
    for (auto& line : lines_) {
      while (line.size() < (size_t)x_size) {
        line.push_back(filler());
      }
    }
  }

  void ComputeRequirement() override {
    requirement_.min_x = 0;
    requirement_.min_y = 0;
    requirement_.flex_grow_x = 0;
    requirement_.flex_grow_y = 0;
    requirement_.flex_shrink_x = 0;
    requirement_.flex_shrink_y = 0;

    for (auto& line : lines_) {
      for (auto& cell : line) {
        cell->ComputeRequirement();

        // Determine focus based on the focused child.
        if (requirement_.selection >= cell->requirement().selection)
          continue;
        requirement_.selection = cell->requirement().selection;
        requirement_.selected_box = cell->requirement().selected_box;
        requirement_.selected_box.x_min += requirement_.min_x;
        requirement_.selected_box.x_max += requirement_.min_x;
      }
    }

    // Work on the x-axis.
    for (int x = 0; x < x_size; ++x) {
      int min_x = 0;
      for (int y = 0; y < y_size; ++y)
        min_x = std::max(min_x, lines_[y][x]->requirement().min_x);
      requirement_.min_x += min_x;
    }

    // Work on the y-axis.
    for (int y = 0; y < y_size; ++y) {
      int min_y = 0;
      for (int x = 0; x < x_size; ++x)
        min_y = std::max(min_y, lines_[y][x]->requirement().min_y);
      requirement_.min_y += min_y;
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    box_helper::Element init;
    init.min_size = 0;
    init.flex_grow = 1024;
    init.flex_shrink = 1024;
    std::vector<box_helper::Element> elements_x(x_size, init);
    std::vector<box_helper::Element> elements_y(y_size, init);

    for (int y = 0; y < y_size; ++y) {
      for (int x = 0; x < x_size; ++x) {
        const auto& cell = lines_[y][x];
        const auto& requirement = cell->requirement();
        auto& e_x = elements_x[x];
        auto& e_y = elements_y[y];
        e_x.min_size = std::max(e_x.min_size, requirement.min_x);
        e_y.min_size = std::max(e_y.min_size, requirement.min_y);
        e_x.flex_grow = std::min(e_x.flex_grow, requirement.flex_grow_x);
        e_y.flex_grow = std::min(e_y.flex_grow, requirement.flex_grow_y);
        e_x.flex_shrink = std::min(e_x.flex_shrink, requirement.flex_shrink_x);
        e_y.flex_shrink = std::min(e_y.flex_shrink, requirement.flex_shrink_y);
      }
    }

    int target_size_x = box.x_max - box.x_min + 1;
    int target_size_y = box.y_max - box.y_min + 1;
    box_helper::Compute(&elements_x, target_size_x);
    box_helper::Compute(&elements_y, target_size_y);

    Box box_y = box;
    int y = box_y.y_min;
    for (int iy = 0; iy < y_size; ++iy) {
      box_y.y_min = y;
      y += elements_y[iy].size;
      box_y.y_max = y - 1;

      Box box_x = box_y;
      int x = box_x.x_min;
      for (int ix = 0; ix < x_size; ++ix) {
        box_x.x_min = x;
        x += elements_x[ix].size;
        box_x.x_max = x - 1;
        lines_[iy][ix]->SetBox(box_x);
      }
    }
  }

  void Render(Screen& screen) override {
    for (auto& line : lines_) {
      for (auto& cell : line)
        cell->Render(screen);
    }
  }

  int x_size = 0;
  int y_size = 0;
  std::vector<Elements> lines_;
};

/// @brief A container displaying a grid of elements.
/// @param lines A list of lines, each line being a list of elements.
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// auto cell = [](const char* t) { return text(t) | border; };
/// auto document = gridbox({
///   {cell("north-west") , cell("north")  , cell("north-east")} ,
///   {cell("west")       , cell("center") , cell("east")}       ,
///   {cell("south-west") , cell("south")  , cell("south-east")} ,
/// });
/// ```
/// Output:
/// ```
///╭──────────╮╭──────╮╭──────────╮
///│north-west││north ││north-east│
///╰──────────╯╰──────╯╰──────────╯
///╭──────────╮╭──────╮╭──────────╮
///│west      ││center││east      │
///╰──────────╯╰──────╯╰──────────╯
///╭──────────╮╭──────╮╭──────────╮
///│south-west││south ││south-east│
///╰──────────╯╰──────╯╰──────────╯
/// ```
Element gridbox(std::vector<Elements> lines) {
  return std::make_shared<GridBox>(std::move(lines));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
