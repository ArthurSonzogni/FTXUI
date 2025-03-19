// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for max, min
#include <cstddef>    // for size_t
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

namespace {

// Accumulate the values of a list U[n] into v[n]. So that:
// V[0] = 0;
// V[n+1] = v[n] + U[n]
// return the sum of U[n].
int Integrate(std::vector<int>& elements) {
  int accu = 0;
  for (auto& i : elements) {
    const int old_accu = accu;
    accu += i;
    i = old_accu;
  }
  return accu;
}

class GridBox : public Node {
 public:
  explicit GridBox(std::vector<Elements> lines) : lines_(std::move(lines)) {
    y_size = static_cast<int>(lines_.size());
    for (const auto& line : lines_) {
      x_size = std::max(x_size, int(line.size()));
    }

    // Fill in empty cells, in case the user did not used the API correctly:
    for (auto& line : lines_) {
      while (line.size() < size_t(x_size)) {
        line.push_back(filler());
      }
    }
  }

  void ComputeRequirement() override {
    requirement_ = Requirement{};
    for (auto& line : lines_) {
      for (auto& cell : line) {
        cell->ComputeRequirement();
      }
    }

    // Compute the size of each columns/row.
    std::vector<int> size_x(x_size, 0);
    std::vector<int> size_y(y_size, 0);
    for (int x = 0; x < x_size; ++x) {
      for (int y = 0; y < y_size; ++y) {
        size_x[x] = std::max(size_x[x], lines_[y][x]->requirement().min_x);
        size_y[y] = std::max(size_y[y], lines_[y][x]->requirement().min_y);
      }
    }

    requirement_.min_x = Integrate(size_x);
    requirement_.min_y = Integrate(size_y);

    // Forward the focused/focused child state:
    for (int x = 0; x < x_size; ++x) {
      for (int y = 0; y < y_size; ++y) {
        if (requirement_.focused.enabled ||
            !lines_[y][x]->requirement().focused.enabled) {
          continue;
        }
        requirement_.focused = lines_[y][x]->requirement().focused;
        requirement_.focused.box.Shift(size_x[x], size_y[y]);
      }
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    box_helper::Element init;
    init.min_size = 0;
    init.flex_grow = 1024;    // NOLINT
    init.flex_shrink = 1024;  // NOLINT
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

    const int target_size_x = box.x_max - box.x_min + 1;
    const int target_size_y = box.y_max - box.y_min + 1;
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
      for (auto& cell : line) {
        cell->Render(screen);
      }
    }
  }

  int x_size = 0;
  int y_size = 0;
  std::vector<Elements> lines_;
};
}  // namespace
   //
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
/// ╭──────────╮╭──────╮╭──────────╮
/// │north-west││north ││north-east│
/// ╰──────────╯╰──────╯╰──────────╯
/// ╭──────────╮╭──────╮╭──────────╮
/// │west      ││center││east      │
/// ╰──────────╯╰──────╯╰──────────╯
/// ╭──────────╮╭──────╮╭──────────╮
/// │south-west││south ││south-east│
/// ╰──────────╯╰──────╯╰──────────╯
/// ```
Element gridbox(std::vector<Elements> lines) {
  return std::make_shared<GridBox>(std::move(lines));
}

}  // namespace ftxui
