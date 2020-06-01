// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.

#include <algorithm>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/util/autoreset.hpp"

namespace ftxui {

// -----------------------------------------------------------------------------

class Select : public Node {
 public:
  Select(std::vector<std::shared_ptr<Node>> children)
      : Node(std::move(children)) {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
    auto& selected_box = requirement_.selected_box;
    selected_box.x_min = 0;
    selected_box.y_min = 0;
    selected_box.x_max = requirement_.min_x;
    selected_box.y_max = requirement_.min_y;
    requirement_.selection = Requirement::SELECTED;
  };

  void SetBox(Box box) override {
    box_ = box;
    children[0]->SetBox(box);
  }
};

Element select(Element child) {
  return std::make_shared<Select>(unpack(std::move(child)));
}

// -----------------------------------------------------------------------------

class Focus : public Select {
 public:
  Focus(std::vector<Element> children) : Select(std::move(children)) {}

  void ComputeRequirement() override {
    Select::ComputeRequirement();
    requirement_.selection = Requirement::FOCUSED;
  };

  void Render(Screen& screen) override {
    Select::Render(screen);
    screen.SetCursor(Screen::Cursor{box_.x_min, box_.y_min});
  }
};

Element focus(Element child) {
  return std::make_shared<Focus>(unpack(std::move(child)));
}

// -----------------------------------------------------------------------------

class Frame : public Node {
 public:
  Frame(std::vector<Element> children) : Node(std::move(children)) {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    int external_dimx = box.x_max - box.x_min;
    int external_dimy = box.y_max - box.y_min;

    int internal_dimx = std::max(requirement_.min_x, external_dimx);
    int internal_dimy = std::max(requirement_.min_y, external_dimy);

    auto& selected_box = requirement_.selected_box;
    int focused_dimx = selected_box.x_max - selected_box.x_min;
    int focused_dimy = selected_box.y_max - selected_box.y_min;
    int dx = selected_box.x_min - external_dimx / 2 + focused_dimx / 2;
    int dy = selected_box.y_min - external_dimy / 2 + focused_dimy / 2;
    dx = std::max(0, std::min(internal_dimx - external_dimx - 1, dx));
    dy = std::max(0, std::min(internal_dimy - external_dimy - 1, dy));

    Box children_box = box;
    children_box.x_min = box.x_min - dx;
    children_box.y_min = box.y_min - dy;
    children_box.x_max = box.x_min + internal_dimx - dx;
    children_box.y_max = box.y_min + internal_dimy - dy;
    children[0]->SetBox(children_box);

    // int dx = box.x_max - box.x_min;
    // int dy = box.y_max - box.y_min;
    // int cdx = std::min(children[0].requirement().min_x

    // Box children_box;
    // children_box.x_min =
    // if (box.x_max - box.x_min >= children[0].requirement().min_x && //

    // box.y_max - box.y_min >= children[0].requirement().min_y) {
    // children_[0]->SetBox(box);
    // dx = 0;
    // dy = 0;
    // return;
    //}

    // Box children_box;
    // children_box.x_min = box.x_min;
    // children_box.y_min = box.x_min;
  }

  void Render(Screen& screen) override {
    AutoReset<Box> stencil(&screen.stencil,
                           Box::Intersection(box_, screen.stencil));
    children[0]->Render(screen);
  }
};

Element frame(Element child) {
  return std::make_shared<Frame>(unpack(std::move(child)));
}

}  // namespace ftxui
