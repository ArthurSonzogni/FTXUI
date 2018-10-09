#include "ftxui/dom/node.hpp"
#include "ftxui/dom/elements.hpp"

namespace ftxui {
namespace dom {

static wchar_t charset[] = L"┌┐└┘─│┬┴┤├";

class Frame : public Node {
 public:
  Frame(Children children) : Node(std::move(children)) {}
  ~Frame() override {}

  void ComputeRequirement() override {
    Node::ComputeRequirement();
    requirement_ = children[0]->requirement();
    requirement_.min.x += 2;
    requirement_.min.y += 2;
    if (children.size() == 2) {
      requirement_.min.x =
          std::max(requirement_.min.x, children[1]->requirement().min.x + 2);
    }
  }

  void SetBox(Box box) override {
    Node::SetBox(box);
    if (children.size() == 2) {
      Box title_box;
      title_box.left = box.left + 1;
      title_box.right = box.right - 1;
      title_box.top = box.top;
      title_box.bottom = box.top;
      children[1]->SetBox(title_box);
    }
    box.left++;
    box.right--;
    box.top++;
    box.bottom--;
    children[0]->SetBox(box);
  }

  void Render(Screen& screen) override {
    // Draw content.
    children[0]->Render(screen);

    // Draw the frame.
    if (box_.left >= box_.right || box_.top >= box_.bottom)
      return;

    screen.at(box_.left, box_.top) = charset[0];
    screen.at(box_.right, box_.top) = charset[1];
    screen.at(box_.left, box_.bottom) = charset[2];
    screen.at(box_.right, box_.bottom) = charset[3];
    for(float x = box_.left + 1; x<box_.right; ++x) {
      screen.at(x, box_.top) = charset[4];
      screen.at(x, box_.bottom) = charset[4];
    }
    for(float y = box_.top + 1; y<box_.bottom; ++y) {
      screen.at(box_.left, y) = charset[5];
      screen.at(box_.right,y) = charset[5];
    }

    // Try to merge with separator.
    for(float x = box_.left + 1; x<box_.right; ++x) {
      if (screen.at(x, box_.top + 1) == charset[5])
        screen.at(x, box_.top) = charset[6];
      if (screen.at(x, box_.bottom - 1) == charset[5])
        screen.at(x, box_.bottom) = charset[7];
    }
    for(float y = box_.top + 1; y<box_.bottom; ++y) {
      if (screen.at(box_.left+1, y) == charset[4])
        screen.at(box_.left, y) = charset[9];
      if (screen.at(box_.right-1, y) == charset[4])
        screen.at(box_.right,y) = charset[8];
    }

    // Draw title.
    if (children.size() == 2)
      children[1]->Render(screen);
  }
};

std::unique_ptr<Node> frame(Child child) {
  return std::make_unique<Frame>(unpack(std::move(child)));
}

std::unique_ptr<Node> frame(Child title, Child content) {
  return std::make_unique<Frame>(unpack(std::move(content), std::move(title)));
}

};  // namespace dom
};  // namespace ftxui
