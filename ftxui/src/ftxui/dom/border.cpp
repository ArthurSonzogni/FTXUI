#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {

using namespace ftxui;

static wchar_t charset[] = L"┌┐└┘─│┬┴┤├";

class Border : public Node {
 public:
  Border(Elements children) : Node(std::move(children)) {}
  ~Border() override {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
    requirement_.min.x += 2;
    requirement_.min.y += 2;
    if (children.size() == 2) {
      requirement_.min.x =
          std::max(requirement_.min.x, children[1]->requirement().min.x + 2);
    }
    requirement_.selected_box.x_min++;
    requirement_.selected_box.x_max++;
    requirement_.selected_box.y_min++;
    requirement_.selected_box.y_max++;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    if (children.size() == 2) {
      Box title_box;
      title_box.x_min = box.x_min + 1;
      title_box.x_max = box.x_max - 1;
      title_box.y_min = box.y_min;
      title_box.y_max = box.y_min;
      children[1]->SetBox(title_box);
    }
    box.x_min++;
    box.x_max--;
    box.y_min++;
    box.y_max--;
    children[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    // Draw content.
    children[0]->Render(screen);

    // Draw the border.
    if (box_.x_min >= box_.x_max || box_.y_min >= box_.y_max)
      return;

    screen.at(box_.x_min, box_.y_min) = charset[0];
    screen.at(box_.x_max, box_.y_min) = charset[1];
    screen.at(box_.x_min, box_.y_max) = charset[2];
    screen.at(box_.x_max, box_.y_max) = charset[3];
    for(float x = box_.x_min + 1; x<box_.x_max; ++x) {
      screen.at(x, box_.y_min) = charset[4];
      screen.at(x, box_.y_max) = charset[4];
    }
    for(float y = box_.y_min + 1; y<box_.y_max; ++y) {
      screen.at(box_.x_min, y) = charset[5];
      screen.at(box_.x_max,y) = charset[5];
    }

    // Draw title.
    if (children.size() == 2)
      children[1]->Render(screen);
  }
};

std::unique_ptr<Node> border(Element child) {
  return std::make_unique<Border>(unpack(std::move(child)));
}

std::unique_ptr<Node> window(Element title, Element content) {
  return std::make_unique<Border>(unpack(std::move(content), std::move(title)));
}

Decorator boxed() {
  return [](Element child) {
    return border(std::move(child));
  };
}

};  // namespace ftxui
