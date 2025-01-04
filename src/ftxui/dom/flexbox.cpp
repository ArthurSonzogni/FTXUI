// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <algorithm>  // for min, max
#include <cstddef>    // for size_t
#include <memory>  // for __shared_ptr_access, shared_ptr, allocator_traits<>::value_type, make_shared
#include <utility>  // for move, swap
#include <vector>   // for vector

#include "ftxui/dom/elements.hpp"  // for Element, Elements, flexbox, hflow, vflow
#include "ftxui/dom/flexbox_config.hpp"  // for FlexboxConfig, FlexboxConfig::Direction, FlexboxConfig::Direction::Column, FlexboxConfig::AlignContent, FlexboxConfig::Direction::ColumnInversed, FlexboxConfig::Direction::Row, FlexboxConfig::JustifyContent, FlexboxConfig::Wrap, FlexboxConfig::AlignContent::FlexStart, FlexboxConfig::Direction::RowInversed, FlexboxConfig::JustifyContent::FlexStart, FlexboxConfig::Wrap::Wrap
#include "ftxui/dom/flexbox_helper.hpp"  // for Block, Global, Compute
#include "ftxui/dom/node.hpp"            // for Node, Elements, Node::Status
#include "ftxui/dom/requirement.hpp"     // for Requirement
#include "ftxui/screen/box.hpp"          // for Box

namespace ftxui {

namespace {
void Normalize(FlexboxConfig::Direction& direction) {
  switch (direction) {
    case FlexboxConfig::Direction::Row:
    case FlexboxConfig::Direction::RowInversed: {
      direction = FlexboxConfig::Direction::Row;
    } break;
    case FlexboxConfig::Direction::Column:
    case FlexboxConfig::Direction::ColumnInversed: {
      direction = FlexboxConfig::Direction::Column;
    } break;
  }
}

void Normalize(FlexboxConfig::AlignContent& align_content) {
  align_content = FlexboxConfig::AlignContent::FlexStart;
}

void Normalize(FlexboxConfig::JustifyContent& justify_content) {
  justify_content = FlexboxConfig::JustifyContent::FlexStart;
}

void Normalize(FlexboxConfig::Wrap& wrap) {
  wrap = FlexboxConfig::Wrap::Wrap;
}

FlexboxConfig Normalize(FlexboxConfig config) {
  Normalize(config.direction);
  Normalize(config.wrap);
  Normalize(config.justify_content);
  Normalize(config.align_content);
  return config;
}

class Flexbox : public Node {
 public:
  Flexbox(Elements children, FlexboxConfig config)
      : Node(std::move(children)),
        config_(config),
        config_normalized_(Normalize(config)) {
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 0;

    if (IsColumnOriented()) {
      std::swap(requirement_.flex_grow_x, requirement_.flex_grow_y);
    }
  }

  bool IsColumnOriented() const {
    return config_.direction == FlexboxConfig::Direction::Column ||
           config_.direction == FlexboxConfig::Direction::ColumnInversed;
  }

  void Layout(flexbox_helper::Global& global,
              bool compute_requirement = false) {
    global.blocks.reserve(children_.size());
    for (auto& child : children_) {
      flexbox_helper::Block block;
      block.min_size_x = child->requirement().min_x;
      block.min_size_y = child->requirement().min_y;
      if (!compute_requirement) {
        block.flex_grow_x = child->requirement().flex_grow_x;
        block.flex_grow_y = child->requirement().flex_grow_y;
        block.flex_shrink_x = child->requirement().flex_shrink_x;
        block.flex_shrink_y = child->requirement().flex_shrink_y;
      }
      global.blocks.push_back(block);
    }

    flexbox_helper::Compute(global);
  }

  void ComputeRequirement() override {
    for (auto& child : children_) {
      child->ComputeRequirement();
    }
    global_ = flexbox_helper::Global();
    global_.config = config_normalized_;
    if (IsColumnOriented()) {
      global_.size_x = 100000;  // NOLINT
      global_.size_y = asked_;
    } else {
      global_.size_x = asked_;
      global_.size_y = 100000;  // NOLINT
    }
    Layout(global_, true);

    // Reset:
    requirement_.selection = Requirement::Selection::NORMAL;
    requirement_.selected_box = Box();
    requirement_.min_x = 0;
    requirement_.min_y = 0;

    if (global_.blocks.empty()) {
      return;
    }

    // Compute the union of all the blocks:
    Box box;
    box.x_min = global_.blocks[0].x;
    box.y_min = global_.blocks[0].y;
    box.x_max = global_.blocks[0].x + global_.blocks[0].dim_x;
    box.y_max = global_.blocks[0].y + global_.blocks[0].dim_y;
    for (auto& b : global_.blocks) {
      box.x_min = std::min(box.x_min, b.x);
      box.y_min = std::min(box.y_min, b.y);
      box.x_max = std::max(box.x_max, b.x + b.dim_x);
      box.y_max = std::max(box.y_max, b.y + b.dim_y);
    }
    requirement_.min_x = box.x_max - box.x_min;
    requirement_.min_y = box.y_max - box.y_min;

    // Find the selection:
    for (size_t i = 0; i < children_.size(); ++i) {
      if (requirement_.selection >= children_[i]->requirement().selection) {
        continue;
      }
      requirement_.selection = children_[i]->requirement().selection;
      Box selected_box = children_[i]->requirement().selected_box;

      // Shift |selected_box| according to its position inside this component:
      auto& b = global_.blocks[i];
      selected_box.x_min += b.x;
      selected_box.y_min += b.y;
      selected_box.x_max += b.x;
      selected_box.y_max += b.y;
      requirement_.selected_box = Box::Intersection(selected_box, box);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    const int asked_previous = asked_;
    asked_ = std::min(asked_, IsColumnOriented() ? box.y_max - box.y_min + 1
                                                 : box.x_max - box.x_min + 1);
    need_iteration_ = (asked_ != asked_previous);

    flexbox_helper::Global global;
    global.config = config_;
    global.size_x = box.x_max - box.x_min + 1;
    global.size_y = box.y_max - box.y_min + 1;
    Layout(global);

    for (size_t i = 0; i < children_.size(); ++i) {
      auto& child = children_[i];
      auto& b = global.blocks[i];

      Box children_box;
      children_box.x_min = box.x_min + b.x;
      children_box.y_min = box.y_min + b.y;
      children_box.x_max = box.x_min + b.x + b.dim_x - 1;
      children_box.y_max = box.y_min + b.y + b.dim_y - 1;

      const Box intersection = Box::Intersection(children_box, box);
      child->SetBox(intersection);

      need_iteration_ |= (intersection != children_box);
    }
  }

  void Select(Selection& selection) override {
    // If this Node box_ doesn't intersect with the selection, then no
    // selection.
    if (Box::Intersection(selection.GetBox(), box_).IsEmpty()) {
      return;
    }

    Selection selection_lines = IsColumnOriented()
                                    ? selection.SaturateVertical(box_)
                                    : selection.SaturateHorizontal(box_);

    size_t i = 0;
    for (auto& line : global_.lines) {
      Box box;
      box.x_min = box_.x_min + line.x;
      box.x_max = box_.x_min + line.x + line.dim_x - 1;
      box.y_min = box_.y_min + line.y;
      box.y_max = box_.y_min + line.y + line.dim_y - 1;

      // If the line box doesn't intersect with the selection, then no
      // selection.
      if (Box::Intersection(selection.GetBox(), box).IsEmpty()) {
        continue;
      }

      Selection selection_line =
          IsColumnOriented() ? selection_lines.SaturateHorizontal(box)
                             : selection_lines.SaturateVertical(box);

      for (auto& block : line.blocks) {
        children_[i]->Select(selection_line);
        i++;
      }
    }
  }

  void Check(Status* status) override {
    for (auto& child : children_) {
      child->Check(status);
    }

    if (status->iteration == 0) {
      asked_ = 6000;  // NOLINT
      need_iteration_ = true;
    }

    status->need_iteration |= need_iteration_;
  }

  int asked_ = 6000;  // NOLINT
  bool need_iteration_ = true;
  const FlexboxConfig config_;
  const FlexboxConfig config_normalized_;
  flexbox_helper::Global global_;
};

}  // namespace

/// @brief A container displaying elements on row/columns and capable of
/// wrapping on the next column/row when full.
/// @param children The elements in the container
/// @param config The option
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// flexbox({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// }, FlexboxConfig()
//       .Set(FlexboxConfig::Direction::Column)
//       .Set(FlexboxConfig::Wrap::WrapInversed)
//       .SetGapMainAxis(1)
//       .SetGapCrossAxis(1)
//  )
/// ```
Element flexbox(Elements children, FlexboxConfig config) {
  return std::make_shared<Flexbox>(std::move(children), config);
}

/// @brief A container displaying elements in rows from left to right. When
/// filled, it starts on a new row below.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// hflow({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// });
/// ```
Element hflow(Elements children) {
  return flexbox(std::move(children), FlexboxConfig());
}

/// @brief A container displaying elements in rows from top to bottom. When
/// filled, it starts on a new columns on the right.
/// filled, it starts on a new row.
/// is full, it starts a new row.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// vflow({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// });
/// ```
Element vflow(Elements children) {
  return flexbox(std::move(children),
                 FlexboxConfig().Set(FlexboxConfig::Direction::Column));
}

}  // namespace ftxui
